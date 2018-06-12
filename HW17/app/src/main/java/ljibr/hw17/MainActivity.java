package ljibr.hw17;
// libraries

import android.Manifest;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.HorizontalScrollView;
import android.widget.TextView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.hoho.android.usbserial.driver.CdcAcmSerialDriver;
import com.hoho.android.usbserial.driver.ProbeTable;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;
    private SeekBar myControl;

    public static int slidebar;
    public float COM = 0;

    //USB Global vars
    private UsbManager manager;
    private UsbSerialPort sPort;
    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private SerialInputOutputManager mSerialIoManager;

    static long prevtime = 0; // for FPS calculation

    //USB Initializations


    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mTextView = (TextView) findViewById(R.id.cameraStatus);

        // see if the app has permission to use the camera
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
            mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
            mSurfaceHolder = mSurfaceView.getHolder();

            mTextureView = (TextureView) findViewById(R.id.textureview);
            mTextureView.setSurfaceTextureListener(this);

            // set the paintbrush for writing text on the image
            paint1.setColor(0xffff0000); // red
            paint1.setTextSize(24);

            mTextView.setText("started camera");
        } else {
            mTextView.setText("no camera permissions");
        }

        myControl = (SeekBar) findViewById(R.id.seek1);
        setMyControlListener();

        manager = (UsbManager) getSystemService(Context.USB_SERVICE);

    }
    

    private void setMyControlListener() {
        myControl.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;
                slidebar = progress;

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
             private final SerialInputOutputManager.Listener mListener =
                        new SerialInputOutputManager.Listener() {
                            @Override
                            public void onRunError(Exception e) {

                            }

                            @Override
                            public void onNewData(final byte[] data) {
                                MainActivity.this.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        MainActivity.this.updateReceivedData(data);
                                    }
                                });
                            }
                        };

                @Override
                protected void onPause(){
                    super.onPause();
                    stopIoManager();
                    if(sPort != null){
                        try{
                            sPort.close();
                        } catch (IOException e){ }
                        sPort = null;
                    }
                    finish();
                }

                @Override
                protected void onResume() {
                    super.onResume();

                    ProbeTable customTable = new ProbeTable();
                    customTable.addProduct(0x04D8,0x000A, CdcAcmSerialDriver.class);
                    UsbSerialProber prober = new UsbSerialProber(customTable);

                    final List<UsbSerialDriver> availableDrivers = prober.findAllDrivers(manager);

                    if(availableDrivers.isEmpty()) {
                        //check
                        return;
                    }

                    UsbSerialDriver driver = availableDrivers.get(0);
                    sPort = driver.getPorts().get(0);

                    if (sPort == null){
                        //check
                    }else{
                        final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
                        UsbDeviceConnection connection = usbManager.openDevice(driver.getDevice());
                        if (connection == null){
                            //check
                            PendingIntent pi = PendingIntent.getBroadcast(this, 0, new Intent("com.android.example.USB_PERMISSION"), 0);
                            usbManager.requestPermission(driver.getDevice(), pi);
                            return;
                        }

                        try {
                            sPort.open(connection);
                            sPort.setParameters(9600, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);

                        }catch (IOException e) {
                            //check
                            try{
                                sPort.close();
                            } catch (IOException e1) { }
                            sPort = null;
                            return;
                        }
                    }
                    onDeviceStateChange();
                }

                private void stopIoManager(){
                    if(mSerialIoManager != null) {
                        mSerialIoManager.stop();
                        mSerialIoManager = null;
                    }
                }

                private void startIoManager() {
                    if(sPort != null){
                        mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
                        mExecutor.submit(mSerialIoManager);
                    }
                }

                private void onDeviceStateChange(){
                    stopIoManager();
                    startIoManager();
                }

                private void updateReceivedData(byte[] data) {
                    //do something with received data

                    //for displaying:
                    String rxString = null;
                    try {
                        rxString = new String(data, "UTF-8"); // put the data you got into a string
                        //myTextView3.append(rxString);
                        //HorizontalScrollView myScrollView;
                        //myScrollView.fullScroll(View.FOCUS_DOWN);
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    }
                    String sendString = String.valueOf(COM) + '\n';
                            try {
                                sPort.write(sendString.getBytes(), 10); // 10 is the timeout
                            } catch (IOException e) { }
                }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        //parameters.setAutoExposureLock(true); // keep the white balance constant
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    
    
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // every time there is a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {
            int thresh = 100 - slidebar; // comparison value
            int[] pixels = new int[bmp.getWidth()]; // pixels[] is the RGBA data

            //for (int startY = 0; startY < bmp.getHeight(); startY++) {
                int startY = 200; // which row in the bitmap to analyze to read
                bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);

                int sum_mr = 0; // the sum of the mass times the radius
                int sum_m = 0; // the sum of the masses
                // in the row, see if there is more green than red
                for (int i = 0; i < bmp.getWidth(); i++) {
                    if ((green(pixels[i]) - red(pixels[i])) > thresh) {
                        pixels[i] = rgb(0, 255, 0); // over write the pixel with pure green
                        sum_m = sum_m + green(pixels[i]);
                        sum_mr = sum_mr + i * green(pixels[i]);
                        //sum_m = sum_m + green(pixels[i])+red(pixels[i])+blue(pixels[i]);
                        //sum_mr = sum_mr + (green(pixels[i])+red(pixels[i])+blue(pixels[i]))*i;
                    }
                }
                if (sum_m > 5) {
                    COM = sum_mr / sum_m;
                } else {
                    COM = 0;
                }
                //startY = startY + 3; //Only use some of the y values


                // update the row
                bmp.setPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);
            //}

        }


        // draw a circle at some position
        canvas.drawCircle(COM, 240, 5, paint1); // x position, y position, diameter, color

        // write the pos as text
        canvas.drawText("COM = " + COM, 10, 200, paint1);
        c.drawBitmap(bmp, 0, 0, null);
        mSurfaceHolder.unlockCanvasAndPost(c);

        // calculate the FPS to see how fast the code is running
        long nowtime = System.currentTimeMillis();
        long diff = nowtime - prevtime;
        mTextView.setText("COM " + COM);
        prevtime = nowtime;
    }


}