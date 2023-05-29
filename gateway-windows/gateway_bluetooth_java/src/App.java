import java.io.IOException;

import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
//import javax.microedition.io.*;
//import javax.microedition.io.StreamConnection;
import javax.microedition.io.StreamConnectionNotifier;



public class App {
    private static final String FIRST_PART_URL_CONNECTION = "btspp://localhost:";
    private static final String UUID = "0000110100001000800000805F9B34FB"; // UUID genérico para SPP (Serial Port Profile)
    private static final String SECOND_PART_URL_CONNECTION = ";name=RemoteBluetooth";
    private static ReceiverThread receiverThread;
    private static TransmitterThread transmitterThread;   
    private static ClientTelnet clientTelnet;

    
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
            //Creo la instancia del Cliente de Telnet para poder comunicarme con qemu dentro de docker
            clientTelnet=new ClientTelnet();

            //Establezco la configuración del bluetooth
            LocalDevice blueDevice = LocalDevice.getLocalDevice();
            blueDevice.setDiscoverable(DiscoveryAgent.GIAC);
        
            String url = FIRST_PART_URL_CONNECTION + UUID.toString()+ SECOND_PART_URL_CONNECTION ;
            notifier = (StreamConnectionNotifier) Connector.open(url);    
       
            System.out.println("waiting for connection...");
       
            connection = notifier.acceptAndOpen();
            System.out.println("Conenction created");

            //Creo los hilos Receptores y transmisores del bluetooth
            receiverThread= new ReceiverThread(connection,clientTelnet);
            transmitterThread =new TransmitterThread(connection,clientTelnet);

            receiverThread.start();
            transmitterThread.start();
            
            //creo el handler que captura el CTRL+C
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