import java.io.BufferedReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.io.InputStreamReader;

public class App 
{
    public static void main(String[] args) {
        String server = "localhost"; // Dirección IP o nombre de dominio del servidor Telnet
        int port = 7777; // Puerto del servidor Telnet

        try 
        {
            System.out.println("empezando");

            // Establecer la conexión TCP con el servidor Telnet
            Socket socket = new Socket(server, port);

            // Crear los objetos para enviar y recibir datos a través de la conexión
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            
            System.out.println("Leyendo teclado....");
            
            // Leer datos de entrada del usuario desede el teclado
            BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in));
            String input = userInput.readLine();
            
            // Enviar los comandos al servidor Telnet
            while ((input) != null)
            {
                //envio los datos a al telnet
                out.println(input);
                
                // Leo la respuesta del servidor Telnet
                String response = in.readLine();
                
                System.out.println("Server response: " + response);
                System.out.println("Server responselenght: " + response.length());
                
                //vuelvo a leer datos de entrada del usuario desede el teclado
                input = userInput.readLine();
            }

            // Cerrar la conexión
            socket.close();
        } 
        catch (Exception e) 
        {
            e.printStackTrace();
        }
    }
}
