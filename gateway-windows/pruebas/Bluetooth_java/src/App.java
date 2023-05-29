import java.io.IOException;

import javax.bluetooth.*;
import javax.microedition.io.*;
import javax.microedition.io.StreamConnection;



public class App {
    private static final String FIRST_PART_URL_CONNECTION = "btspp://localhost:";
    private static final String UUID = "0000110100001000800000805F9B34FB"; // UUID genérico para SPP (Serial Port Profile)
    private static final String SECOND_PART_URL_CONNECTION = ";name=RemoteBluetooth";
    private static ReceiverThread receiverThread;
    private static TransmitterThread transmitterThread;   


    
    private static void catchSignal()
    {
        // Capturar la señal de interrupción (CTRL+C)
        Runtime.getRuntime().addShutdownHook(new Thread(() -> 
        {
                                      
            try
            {   
                // Detener los hilos hijos
                receiverThread.interrupt();
                transmitterThread.interrupt();
                
                // Esperar a que los hilos hijos terminen
                receiverThread.join();
                transmitterThread.join();
                
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
    
            System.out.println("Programa finalizado");       
     
        }));

     
    }

    public static void main(String[] args) throws InterruptedException {
        StreamConnectionNotifier notifier = null;
        StreamConnection connection = null;
     

        try
        {
            LocalDevice blueDevice = LocalDevice.getLocalDevice();
            blueDevice.setDiscoverable(DiscoveryAgent.GIAC);
        
            String url = FIRST_PART_URL_CONNECTION + UUID.toString()+ SECOND_PART_URL_CONNECTION ;
            notifier = (StreamConnectionNotifier) Connector.open(url);    
       
            System.out.println("waiting for connection...");
       
            connection = notifier.acceptAndOpen();
            System.out.println("Conenction created");

            receiverThread= new ReceiverThread(connection);
            transmitterThread =new TransmitterThread(connection);

            receiverThread.start();
            transmitterThread.start();
            
            catchSignal();
            
               
        }
        catch (BluetoothStateException e) 
        {
            System.out.println("ModuleBluetooth: Error getting the bluetooth device");
        }
        catch (IOException e) 
        {
            System.out.println("Can not create the connection");
        }
        
    }   
    

}