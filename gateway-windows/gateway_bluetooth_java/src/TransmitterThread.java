import java.io.IOException;
import java.io.OutputStream;

import javax.microedition.io.StreamConnection;

public class TransmitterThread extends Thread
{
    private StreamConnection connection;
    private OutputStream  outputStream;
    private ClientTelnet clientTelnet;
               
        public TransmitterThread(StreamConnection connection, ClientTelnet client)
        {
            try
            {

                this.outputStream =connection.openOutputStream();
                this.connection=connection; 
                this.clientTelnet=client;
            }
            catch (Exception e) 
            {
                System.out.println("Error: "+ e.getMessage().toString());
            }
        }
        //metodo run del hilo, que va a entrar en una espera activa para recibir los msjs del HC05
        public void run()
        {       
            int i=0;
            String message="";     
            //el hilo secundario se queda esperando para evniar mensajes
            while (!Thread.currentThread().isInterrupted())
            {
                try 
                {     
                    //recibe los datos de Qemu
                    message = clientTelnet.receiveDataFromQemu();                

                    //le transmite al bluetooth los datos que fueron recibidos desde qemu
                    sendDataToBluetooth(message);
                    Thread.sleep(10);
                }
                catch (InterruptedException e) 
                {
                    //e.printStackTrace();
                    System.out.println("Se interrumpio transmisor");
                    break;
                }
            }
            //System.out.println("Cerrando socket transmiscion...");
            closeConnection();
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

        
        public void closeConnection()
        {
            try 
            {
                connection.close();
                System.out.println("Cerrando socket transmiscion...");
            } catch (IOException e) 
            {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
 
    
}
