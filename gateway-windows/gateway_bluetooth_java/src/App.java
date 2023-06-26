import java.io.IOException;

import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
//import javax.microedition.io.*;
//import javax.microedition.io.StreamConnection;
import javax.microedition.io.StreamConnectionNotifier;



public class App {
    private static ReceiverThread receiverThread;
    private static TransmitterThread transmitterThread;   
    private static ClientTelnet clientTelnet;
    private static ClientBluetooth clientBluetooth;
    
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

    public static void main(String[] args) throws InterruptedException 
    {
        

            //Creo la instancia del Cliente de Telnet para poder comunicarme con qemu dentro de docker
            clientTelnet    = new ClientTelnet();
            clientBluetooth = new ClientBluetooth();

            //Creo los hilos Receptores y transmisores del bluetooth
            receiverThread= new ReceiverThread(clientTelnet,clientBluetooth);
            transmitterThread =new TransmitterThread(clientTelnet, clientBluetooth);

            receiverThread.start();
            transmitterThread.start();
            
            //creo el handler que captura el CTRL+C
            catchSignal();
            
    }   
    

}