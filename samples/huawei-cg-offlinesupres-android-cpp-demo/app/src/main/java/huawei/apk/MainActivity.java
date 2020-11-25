package huawei.apk;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import androidx.appcompat.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    private Button ChoosePictureBtn;
    private Button btnInit, btnUninit, btnQuerySS, btnQueryIE, btnSSSync, btnIESync, btnSSAsync, btnIEAsync;
    private Button btnCommandZero, btnCommandOne, btnCommandTwo, btnCommandThree, btnCommandFour, btnCommandFive, btnCommandSix, btnCommandSeven, btnCommandEight, btnCommandNine, btnCommandTen, btnTM;
    private String imageFilePathName = null;
    private TextView mInfoView = null;
    public static huawei.apk.Interface appInterface = new huawei.apk.Interface();

    //权限
    private static final int PERMISSION_CODE = 0x00010001;
    private static Context mainActivityContext;
    private boolean checkPermissionAllGranted(String[] permissions) {
        for (String permission : permissions) {
            if (checkSelfPermission(permission) != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    //requerest ID
    private int REQUEST_CODE_SELECT_PIC = 120;

    //选择图片
    private void selectPicture() {
        Intent intent = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        intent.setAction(Intent.ACTION_PICK);
        intent.setDataAndType(MediaStore.Images.Media.EXTERNAL_CONTENT_URI,"image/*");
        // 这个函数会自动调用下面的函数 onActivityResult
        startActivityForResult(intent, REQUEST_CODE_SELECT_PIC);
        return;
    }

    //拍照默认路径
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        //选择图片
        if (requestCode == REQUEST_CODE_SELECT_PIC) {
            if (data != null) {
                Uri img = data.getData();
                String[] imgPath = {MediaStore.Images.Media.DATA};
                Cursor cur = getContentResolver().query(img,
                        imgPath, null, null, null);
                if (cur != null) {
                    cur.moveToFirst();
                    int index = cur.getColumnIndex(imgPath[0]);
                    String imageFile = cur.getString(index);
                    cur.close();
                    imageFilePathName = imageFile;

                } else {
                    imageFilePathName = null;
                }
            }
            Log.i("MainActivity", " choose imageFilePath : " + imageFilePathName);
        }

        if (resultCode != Activity.RESULT_OK) {
            return;
        }
    }

    public static boolean checkPermission(Context context, String[] permissions) {
        PackageManager packageManager = context.getPackageManager();
        String packageName = context.getPackageName();

        for (String permission : permissions) {
            int per = packageManager.checkPermission(permission, packageName);
            if (PackageManager.PERMISSION_DENIED == per) {
                Log.i("MainActivity", "required permission not granted . permission = " + permission);
                return false;
            }
        }
        Log.i("MainActivity", "required permission granted .");
        return true;
    }

    public String debugStr = "";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //permission
        final String[] permissions = new String[]{Manifest.permission.CAMERA, Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
        if(!checkPermissionAllGranted(permissions)) {
            requestPermissions(permissions, PERMISSION_CODE);
        }
        ChoosePictureBtn = (Button)findViewById(R.id.ChoosePicture);
        btnInit = (Button) findViewById(R.id.init);
        btnUninit = (Button) findViewById(R.id.uninit);
        btnQuerySS = (Button) findViewById(R.id.queryss);
        btnQueryIE = (Button) findViewById(R.id.queryie);
        btnSSSync = (Button) findViewById(R.id.sssync);
        btnIESync = (Button) findViewById(R.id.iesync);
        btnSSAsync = (Button) findViewById(R.id.ssasync);
        btnIEAsync = (Button) findViewById(R.id.ieasync);
        btnCommandZero = (Button) findViewById(R.id.numZero);
        btnCommandOne = (Button) findViewById(R.id.numOne);
        btnCommandTwo = (Button) findViewById(R.id.numTwo);
        btnCommandThree = (Button) findViewById(R.id.numThree);
        btnCommandFour = (Button) findViewById(R.id.numFour);
        btnCommandFive = (Button) findViewById(R.id.numFive);
        btnCommandSix = (Button) findViewById(R.id.numSix);
        btnCommandSeven = (Button) findViewById(R.id.numSeven);
        btnCommandEight = (Button) findViewById(R.id.numEight);
        btnCommandNine = (Button) findViewById(R.id.numNine);
        btnCommandTen = (Button) findViewById(R.id.numTen);
        btnTM = (Button) findViewById(R.id.tm);
        mInfoView = findViewById(R.id.gk_info_panel);

        //Button ChoosePictureBtn
        ChoosePictureBtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                if(checkPermission(getApplicationContext(),permissions)) {
                    imageFilePathName = null;
                    selectPicture();
                    debugStr = "Image Selected";
                    mInfoView.setText(debugStr);
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                    //Toast.makeText(getApplication(), "Please check permissions.", Toast.LENGTH_SHORT).show();
                }
            }
        });

        btnSSSync.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    if(imageFilePathName == null) {
                        debugStr = "Please select one photo first.";
                        mInfoView.setText(debugStr);
                    }else{
                        boolean ret = appInterface.NativeExecute(0, imageFilePathName);
                        if(!ret){
                            debugStr = "\nSuperSampling\nFails.\nImage Path: " + imageFilePathName;
                        }else{
                            debugStr = "\nSuperSampling\nSucceeds.\nImage Path: " + imageFilePathName;
                        }
                        mInfoView.setText(debugStr);
                    }
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });

        btnIESync.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    if(imageFilePathName == null) {
                        debugStr = "Please select one photo first.";
                        mInfoView.setText(debugStr);
                    }else{
                        boolean ret = appInterface.NativeExecute(1, imageFilePathName);
                        if(!ret){
                            debugStr = "\nImageEnhancing\nFails.\nImage Path: " + imageFilePathName;
                        }else{
                            debugStr = "\nImageEnhancing\nSucceeds.\nImage Path: " + imageFilePathName;
                        }
                        mInfoView.setText(debugStr);
                    }
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });

        btnSSAsync.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    if(imageFilePathName == null) {
                        debugStr = "Please select one photo first.";
                        mInfoView.setText(debugStr);
                    }else{
                        boolean ret = appInterface.NativeExecute(2, imageFilePathName);
                        if(!ret){
                            debugStr = "\nSuperSampling\nFails.\nImage Path: " + imageFilePathName;
                        }else{
                            debugStr = "\nSuperSampling\nSucceeds.\nImage Path: " + imageFilePathName;
                        }
                        mInfoView.setText(debugStr);
                    }
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });

        btnIEAsync.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    if(imageFilePathName == null) {
                        debugStr = "Please select one photo first.";
                        mInfoView.setText(debugStr);
                    }else{
                        boolean ret = appInterface.NativeExecute(3, imageFilePathName);
                        if(!ret){
                            debugStr = "\nImageEnhancing\nFails.\nImage Path: " + imageFilePathName;
                        }else{
                            debugStr = "\nImageEnhancing\nSucceeds.\nImage Path: " + imageFilePathName;
                        }
                        mInfoView.setText(debugStr);
                    }
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });

        // Query
        btnQuerySS.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    if(imageFilePathName == null) {
                        debugStr = "Please select one photo first.";
                        mInfoView.setText(debugStr);
                     }else{
                        char[] data = appInterface.NativeQuery(0, imageFilePathName);
                        debugStr = String.valueOf(data);
                        mInfoView.setText(debugStr);
                    }
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });

        btnQueryIE.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    if(imageFilePathName == null) {
                        debugStr = "Please select one photo first.";
                        mInfoView.setText(debugStr);
                        //Toast.makeText(getApplication(), "Please select one photo first.", Toast.LENGTH_SHORT).show();//请先选择照片 Please select the photo and build model first
                    }else{
                        char[] data = appInterface.NativeQuery(1, imageFilePathName);
                        debugStr = String.valueOf(data);

                        mInfoView.setText(debugStr);
                    }
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                    //Toast.makeText(getApplication(), "Please check permissions.", Toast.LENGTH_SHORT).show();//"Please check software permission."
                }
            }
        });

        // Initialize and Uninitialize
        btnInit.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    boolean ret = appInterface.NativeInitialize();
                    if (ret) {
                        debugStr = "\n Initialized Successfully ";
                    } else {
                        debugStr = "\n Initializing Failed ";
                    }
                    mInfoView.setText(debugStr);
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnUninit.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    boolean ret = appInterface.NativeUninitialize();
                    if (!ret) {
                        debugStr = "\n Un-Initialized Successfully ";
                    } else {
                        debugStr = "\n Un-Initializing Fails ";
                    }
                }else{
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });

        // button reaction for sharpness setting
        btnCommandZero.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.0f);
                    debugStr = "\n Sharpness set to 0.0 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandOne.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.1f);
                    debugStr = "\n Sharpness set to 0.1 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandTwo.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.2f);
                    debugStr = "\n Sharpness set to 0.2 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandThree.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.3f);
                    debugStr = "\n Sharpness set to 0.3 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandFour.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.4f);
                    debugStr = "\n Sharpness set to 0.4 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandFive.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.5f);
                    debugStr = "\n Sharpness set to 0.5 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandSix.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.6f);
                    debugStr = "\n Sharpness set to 0.6 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandSeven.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.7f);
                    debugStr = "\n Sharpness set to 0.7 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandEight.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.8f);
                    debugStr = "\n Sharpness set to 0.8 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandNine.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(0.9f);
                    debugStr = "\n Sharpness set to 0.9 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
        btnCommandTen.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    appInterface.NativeSetSharpness(1.0f);
                    debugStr = "\n Sharpness set to 1.0 ";
                    mInfoView.setText(debugStr);
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });

        btnTM.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(checkPermission(getApplicationContext(),permissions)) {
                    boolean ret = appInterface.NativeSwitchToneMapping();
                    if (ret) {
                        debugStr = "\n Enable Tone-Mapping ";
                        mInfoView.setText(debugStr);
                    } else {
                        debugStr = "\n Disable Tone-Mapping ";
                        mInfoView.setText(debugStr);
                    }
                } else {
                    debugStr = "Please check permissions.";
                    mInfoView.setText(debugStr);
                }
            }
        });
    }
}
