package net.londatiga.android.bluetooth;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;

import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;

import android.app.Activity;
import android.app.ProgressDialog;

import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

/*********************************************************************************************************
 * Activity Principal de la App. Es la primiera activity que se ejecuta cuando el usuario ingresa a la App
 **********************************************************************************************************/

public class MainActivity extends Activity
{
	private TextView txtEstado;
	private Button btnActivar;
	private Button btnEmparejar;
	private Button btnBuscar;

	private ProgressDialog mProgressDlg;

	private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<BluetoothDevice>();

	private BluetoothAdapter mBluetoothAdapter;

	public static final int MULTIPLE_PERMISSIONS = 10; // code you want.

	//se crea un array de String con los permisos a solicitar en tiempo de ejecucion 
	//Esto se debe realizar a partir de Android 6.0, ya que con verdiones anteriores
	//con solo solicitarlos en el Manifest es suficiente
	/*String[] permissions= new String[]{
			Manifest.permission.BLUETOOTH,
			Manifest.permission.BLUETOOTH_ADMIN,
			Manifest.permission.ACCESS_COARSE_LOCATION,
			Manifest.permission.WRITE_EXTERNAL_STORAGE,
			Manifest.permission.READ_PHONE_STATE,
			Manifest.permission.READ_EXTERNAL_STORAGE
			};
*/
	private static String[] PERMISSIONS_STORAGE = {
			Manifest.permission.READ_EXTERNAL_STORAGE,
			Manifest.permission.WRITE_EXTERNAL_STORAGE,
			Manifest.permission.ACCESS_FINE_LOCATION,
			Manifest.permission.ACCESS_COARSE_LOCATION,
			Manifest.permission.ACCESS_LOCATION_EXTRA_COMMANDS,
			Manifest.permission.BLUETOOTH_SCAN,
			Manifest.permission.BLUETOOTH_CONNECT,
			Manifest.permission.BLUETOOTH_PRIVILEGED
	};
	private static String[] PERMISSIONS_LOCATION = {
			Manifest.permission.ACCESS_FINE_LOCATION,
			Manifest.permission.ACCESS_COARSE_LOCATION,
			Manifest.permission.ACCESS_LOCATION_EXTRA_COMMANDS,
			Manifest.permission.BLUETOOTH_SCAN,
			Manifest.permission.BLUETOOTH_CONNECT,
			Manifest.permission.BLUETOOTH_PRIVILEGED
	};

	@Override
	//Metodo On create
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_main);

		//Se definen los componentes del layout
		txtEstado = (TextView) findViewById(R.id.txtEstado);
		btnActivar = (Button) findViewById(R.id.btnActivar);
		btnEmparejar = (Button) findViewById(R.id.btnEmparejar);
		btnBuscar = (Button) findViewById(R.id.btnBuscar);

		//Se crea un adaptador para podermanejar el bluethoot del celular
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

		//Se Crea la ventana de dialogo que indica que se esta buscando dispositivos bluethoot
		mProgressDlg = new ProgressDialog(this);

		mProgressDlg.setMessage("Buscando dispositivos...");
		mProgressDlg.setCancelable(false);

		//se asocia un listener al boton cancelar para la ventana de dialogo ue busca los dispositivos bluethoot
		mProgressDlg.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancelar", btnCancelarDialogListener);

		//
		if (checkPermissions())
		{
			enableComponent();
		}
		enableComponent();
	}

	protected  void enableComponent()
	{
		//se determina si existe bluethoot en el celular
		if (mBluetoothAdapter == null)
		{
			//si el celular no soporta bluethoot
			showUnsupported();
		}
		else
		{
			//si el celular soporta bluethoot, se definen los listener para los botones de la activity
			btnEmparejar.setOnClickListener(btnEmparejarListener);

			btnBuscar.setOnClickListener(btnBuscarListener);

			btnActivar.setOnClickListener(btnActivarListener);

			//se determina si esta activado el bluethoot
			if (mBluetoothAdapter.isEnabled())
			{
				//se informa si esta habilitado
				showEnabled();
			}
			else
			{
				//se informa si esta deshabilitado
				showDisabled();
			}
		}


		//se definen un broadcastReceiver que captura el broadcast del SO cuando captura los siguientes eventos:
		IntentFilter filter = new IntentFilter();

		filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED); //Cambia el estado del Bluethoot (Acrtivado /Desactivado)
		filter.addAction(BluetoothDevice.ACTION_FOUND); //Se encuentra un dispositivo bluethoot al realizar una busqueda
		filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED); //Cuando se comienza una busqueda de bluethoot
		filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED); //cuando la busqueda de bluethoot finaliza

		//se define (registra) el handler que captura los broadcast anterirmente mencionados.
		registerReceiver(mReceiver, filter);
	}
	@Override
	//Cuando se llama al metodo OnPausa se cancela la busqueda de dispositivos bluethoot
	public void onPause()
	{

		if (mBluetoothAdapter != null) {
			if (mBluetoothAdapter.isDiscovering()) {
				mBluetoothAdapter.cancelDiscovery();
			}
		}

		super.onPause();
	}

	@Override
	//Cuando se detruye la Acivity se quita el registro de los brodcast. Apartir de este momento no se
	//recibe mas broadcast del SO. del bluethoot
	public void onDestroy() {
		unregisterReceiver(mReceiver);

		super.onDestroy();
	}

	private void showEnabled() {
		txtEstado.setText("Bluetooth Habilitar");
		txtEstado.setTextColor(Color.BLUE);

		btnActivar.setText("Desactivar");
		btnActivar.setEnabled(true);

		btnEmparejar.setEnabled(true);
		btnBuscar.setEnabled(true);
	}

	private void showDisabled() {
		txtEstado.setText("Bluetooth Deshabilitado");
		txtEstado.setTextColor(Color.RED);

		btnActivar.setText("Activar");
		btnActivar.setEnabled(true);

		btnEmparejar.setEnabled(false);
		btnBuscar.setEnabled(false);
	}

	private void showUnsupported() {
		txtEstado.setText("Bluetooth no es soportado por el dispositivo movil");

		btnActivar.setText("Activar");
		btnActivar.setEnabled(false);

		btnEmparejar.setEnabled(false);
		btnBuscar.setEnabled(false);
	}

	private void showToast(String message) {
		Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
	}

	//Handler que captura los brodacast que emite el SO al ocurrir los eventos del bluethoot
	private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
		public void onReceive(Context context, @NonNull Intent intent) {

			//Atraves del Intent obtengo el evento de Bluethoot que informo el broadcast del SO
			String action = intent.getAction();

			//Si cambio de estado el Bluethoot(Activado/desactivado)
			if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action))
			{
				//Obtengo el parametro, aplicando un Bundle, que me indica el estado del Bluethoot
				final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

				//Si esta activado
				if (state == BluetoothAdapter.STATE_ON)
				{
					showToast("Activar");

					showEnabled();
				}
			}
			//Si se inicio la busqueda de dispositivos bluethoot
			else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action))
			{
				//Creo la lista donde voy a mostrar los dispositivos encontrados
				mDeviceList = new ArrayList<BluetoothDevice>();

				//muestro el cuadro de dialogo de busqueda
				mProgressDlg.show();
			}
			//Si finalizo la busqueda de dispositivos bluethoot
			else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action))
			{
				//se cierra el cuadro de dialogo de busqueda
				mProgressDlg.dismiss();

				//se inicia el activity DeviceListActivity pasandole como parametros, por intent,
				//el listado de dispositovos encontrados
				Intent newIntent = new Intent(MainActivity.this, DeviceListActivity.class);

				newIntent.putParcelableArrayListExtra("device.list", mDeviceList);

				startActivity(newIntent);
			}
			//si se encontro un dispositivo bluethoot
			else if (BluetoothDevice.ACTION_FOUND.equals(action))
			{
				//Se lo agregan sus datos a una lista de dispositivos encontrados
				BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

				mDeviceList.add(device);
				showToast("Dispositivo Encontrado:" + device.getName());
			}
		}
	};



	//Metodo que actua como Listener de los eventos que ocurren en los componentes graficos de la activty
	private View.OnClickListener btnEmparejarListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {

			Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();

			if (pairedDevices == null || pairedDevices.size() == 0)
			{
				showToast("No se encontraron dispositivos emparejados");
			}
			else
			{
				ArrayList<BluetoothDevice> list = new ArrayList<BluetoothDevice>();

				list.addAll(pairedDevices);

				Intent intent = new Intent(MainActivity.this, DeviceListActivity.class);

				intent.putParcelableArrayListExtra("device.list", list);

				startActivity(intent);
			}
		}
	};

	private View.OnClickListener btnBuscarListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			mBluetoothAdapter.startDiscovery();
		}
	};


	private View.OnClickListener btnActivarListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			if (mBluetoothAdapter.isEnabled()) {
				mBluetoothAdapter.disable();

				showDisabled();
			} else {
				Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);

				startActivityForResult(intent, 1000);
			}
		}
	};


	private DialogInterface.OnClickListener btnCancelarDialogListener = new DialogInterface.OnClickListener() {
		@Override
		public void onClick(DialogInterface dialog, int which) {
			dialog.dismiss();

			mBluetoothAdapter.cancelDiscovery();
		}
	};


	//Metodo que chequea si estan habilitados los permisos
/*	private  boolean checkPermissions2() {
		int result;
		List<String> listPermissionsNeeded = new ArrayList<>();

		//Se chequea si la version de Android es menor a la 6
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
			return true;
		}


		for (String p:permissions) {
			result = ContextCompat.checkSelfPermission(this,p);
			if (result != PackageManager.PERMISSION_GRANTED) {
				listPermissionsNeeded.add(p);
			}
		}
		if (!listPermissionsNeeded.isEmpty()) {
			ActivityCompat.requestPermissions(this, listPermissionsNeeded.toArray(new String[listPermissionsNeeded.size()]),MULTIPLE_PERMISSIONS );
			return false;
		}
		return true;
	}
*/
	@Override
	public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
		switch (requestCode) {
			case MULTIPLE_PERMISSIONS: {
				if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
					// permissions granted.
					enableComponent(); // Now you call here what ever you want :)
				} else {
					String perStr = "";
					for (String per : permissions) {
						perStr += "\n" + per;
					}
					// permissions list of don't granted permission
					Toast.makeText(this,"ATENCION: La aplicacion no funcionara " +
									"correctamente debido a la falta de Permisos", Toast.LENGTH_LONG).show();
				}
				return;
			}
		}
	}
	private boolean checkPermissions(){
		int permission1 = ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
		int permission2 = ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN);
		if (permission1 != PackageManager.PERMISSION_GRANTED) {
			// We don't have permission so prompt the user
			ActivityCompat.requestPermissions(this,PERMISSIONS_STORAGE,1);
		}
		else if (permission2 != PackageManager.PERMISSION_GRANTED){
			ActivityCompat.requestPermissions(this,PERMISSIONS_LOCATION,1);
		}
		return false;
	}
}
