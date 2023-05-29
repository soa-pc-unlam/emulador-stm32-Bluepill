import java.io.IOException;
import java.io.InputStream;

import javax.microedition.io.StreamConnection;

public class ReceiverThread extends Thread
{
    private InputStream  inputStream;
    private StreamConnection connection;
    private ClientTelnet clientTelnet;
           
        public ReceiverThread(StreamConnection  connection, ClientTelnet client)
        {
            try
            {

                this.inputStream =connection.openInputStream(); 
                this.connection=connection;
                this.clientTelnet=client;
            }
            catch (Exception e) 
            {
                System.out.println(e.getMessage().toString());
            }
        }
    
        //metodo run del hilo, que va a entrar en una espera activa para recibir los msjs del HC05
        public void run()
        {
            String readMessage;
            //el hilo secundario se queda esperando mensajes del HC05
            while (!Thread.currentThread().isInterrupted())
            {
                //recive datos del bluetooth
                readMessage=receiveDataFromBluetooth();
                
                if(readMessage.equals(null))
                    break;
                //le transmite a qemu los datos que fueron recibidos del bluetooth 
                clientTelnet.sendDataToQemu(readMessage);

                System.out.println(readMessage);
        
            }

           closeConnection();
            
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

        public void closeConnection()
        {
            try 
            {
                
                //cierro el socket del cliente telnet
                clientTelnet.socketClose();
                
                //cierro el socket de bluetooth
                connection.close();
                System.out.println("Cerrando socket recepecion...");
            } catch (IOException e) 
            {
                e.printStackTrace();
            }
        }

}
