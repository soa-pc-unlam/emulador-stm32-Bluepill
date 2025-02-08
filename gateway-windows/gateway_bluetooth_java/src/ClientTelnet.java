import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.io.InputStreamReader;
import java.net.UnknownHostException;

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

        } catch (UnknownHostException e) {
            System.err.println("Host desconocido: " + e.getMessage());
        } catch (IOException e) {
            System.err.println("Error de conexión: " + e.getMessage());
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
    public String receiveDataFromQemu() {
        StringBuilder sb = new StringBuilder();
        
        String resp=null;
        try {
            int character;
            while (true) {
                try {
                    character = in.read();
                    
                    if (character == -1) {
                        System.out.println("Conexión cerrada por el servidor.");
                        break; 
                    }
    
                    sb.append((char) character);
    
                    if (character == '\n') {
                        break; 
                    }
    
        
                    resp=sb.toString();
                } catch (IOException e) {
                    System.out.println("Error de lectura, posible cierre de socket.");
                    break; 
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    
        return resp;
    }
    
    public void socketClose() {
        try {
            if (socket != null) {
                socket.close();
                System.out.println("Bluetooth socket closed successfully");
            }
        } catch (IOException e) {
            System.err.println("Error closing Bluetooth socket: " + e.getMessage());
        }
    }
}
