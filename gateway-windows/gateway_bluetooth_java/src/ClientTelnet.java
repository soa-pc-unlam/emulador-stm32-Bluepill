import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.io.InputStreamReader;

public class ClientTelnet 
{
    private static final String server = "localhost"; // Dirección IP o nombre de dominio del servidor Telnet
    private static final int PORT = 7777; // Puerto del servidor Telnet
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;

    public ClientTelnet()
    {
        try
        {
            

            // Establecer la conexión TCP con el servidor Telnet
            socket = new Socket(server, PORT);

            // Crear los objetos para enviar y recibir datos a través de la conexión
            out = new PrintWriter(socket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

        } 
        catch (Exception e) 
        {
            e.printStackTrace();
        }   

    }

    public   void sendDataToQemu(String msg)
    {   
        try
        {   
           out.println(msg);
        } 
        catch (Exception e) 
        {
            e.printStackTrace(); 
        }
    }  
    
    public  String receiveDataFromQemu()
     {
        StringBuilder sb = new StringBuilder();
        //BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        long startTime = System.currentTimeMillis();
        long timeout = 5000; // Tiempo límite en milisegundos (5 segundos en este caso)

        try {
            int character;
            boolean foundNewLine = false;
            
            while ((character = in.read()) != -1) 
            {
                sb.append((char) character);
                
                if (character == '\n') 
                {
                    foundNewLine = true;
                    break;
                }   
                
                if (System.currentTimeMillis() - startTime > timeout) {
                    break;
                }
            }
            
            /*if (!foundNewLine) {
                System.out.println("Timeout reached. Returning partial result.");
            }*/
        } catch (IOException e) {
            e.printStackTrace();
        }
        
        return sb.toString();
    }

    public void socketClose()
    {
        try
        {
            socket.close();
        }
        catch (Exception e) 
        {
            e.printStackTrace();
        } 
    }
}
