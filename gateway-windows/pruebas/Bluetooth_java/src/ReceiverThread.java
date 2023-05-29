import java.io.IOException;
import java.io.InputStream;

import javax.microedition.io.StreamConnection;

public class ReceiverThread extends Thread
{
    private InputStream  inputStream;
    private StreamConnection connection;
           
        public ReceiverThread(StreamConnection  connection)
        {
            try
            {
                this.inputStream =connection.openInputStream(); 
                this.connection=connection;
                
            }
            catch (Exception e) 
            {
                System.out.println(e.getMessage().toString());
            }
        }
    
        //metodo run del hilo, que va a entrar en una espera activa para recibir los msjs del HC05
        public void run()
        {
            byte[] buffer = new byte[256];
            int bytes;
            
            //el hilo secundario se queda esperando mensajes del HC05
            while (!Thread.currentThread().isInterrupted())
            {
                try
                {
                    //se leen los datos del Bluethoot
                    bytes = inputStream.read(buffer);
                    String readMessage = new String(buffer, 0, bytes);
                    System.out.println(readMessage);

                } catch (IOException e) 
                {
                    System.out.println("Error en recepcion: "+e.getMessage().toString());
                    break;
                }
            }

           closeConnection();
            
        }

        public void closeConnection()
        {
            try 
            {
                connection.close();
                System.out.println("Cerrando socket recepecion...");
            } catch (IOException e) 
            {
                e.printStackTrace();
            }
        }

}
