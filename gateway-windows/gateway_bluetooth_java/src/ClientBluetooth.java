import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.LocalDevice;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.StreamConnectionNotifier;

public class ClientBluetooth {
    private static final String FIRST_PART_URL_CONNECTION = "btspp://localhost:";
    private static final String UUID = "0000110100001000800000805F9B34FB"; // UUID genérico para SPP (Serial Port Profile)
    private static final String SECOND_PART_URL_CONNECTION = ";name=RemoteBluetooth";
    private  StreamConnectionNotifier notifier = null;
    private InputStream  inputStream;
    private OutputStream  outputStream;
    private StreamConnection connection;    


    public ClientBluetooth()
    { 
        try
        {

            //Establezco la configuración del bluetooth
            LocalDevice blueDevice = LocalDevice.getLocalDevice();
            blueDevice.setDiscoverable(DiscoveryAgent.GIAC);
        
            String url = FIRST_PART_URL_CONNECTION + UUID.toString()+ SECOND_PART_URL_CONNECTION ;
            notifier = (StreamConnectionNotifier) Connector.open(url);    
       
            System.out.println("waiting for connection...");
       
            connection = notifier.acceptAndOpen();
            System.out.println("Conenction created");

            this.inputStream  = connection.openInputStream(); 
            this.outputStream = connection.openOutputStream();
            
        }
        catch (Exception e) 
        {
            System.out.println(e.getMessage().toString());
        }
    }

    public String receiveDataFromBluetooth()
    {
        byte[] buffer = new byte[256];
        int bytes;
        String readMessage;

        try
        {
                

            //se leen los datos del Bluethoot
            bytes = inputStream.read(buffer);
            readMessage = new String(buffer, 0, bytes);
            
            
        } catch (IOException e) 
        {
            System.out.println("Error en recepcion: "+e.getMessage().toString());
            return null;
        }
        return readMessage;
    }

    public void sendDataToBluetooth(String input)
    {
        byte[] msgBuffer = input.getBytes();           //converts entered String into bytes
        try 
        {
            outputStream.write(msgBuffer);                //write bytes over BT connection via outstream
        }
        catch (IOException e) 
        {
            System.out.println("Error en lectura");

        }
    }

    public void socketClose()
    {
        //cierro el socket de bluetooth
        try {
            connection.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }        
}
