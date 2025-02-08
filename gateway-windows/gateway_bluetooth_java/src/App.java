public class App {
    private static ReceiverThread receiverThread;
    private static TransmitterThread transmitterThread;   
    private static ClientTelnet clientTelnet;
    private static ClientBluetooth clientBluetooth;
    private static void catchSignal() {
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            try {
                System.out.println("Closing Application...");
                
                receiverThread.interrupt();
                transmitterThread.interrupt();
                
                receiverThread.join();
                transmitterThread.join();
                
                clientTelnet.socketClose();
                clientBluetooth.socketClose();
                
                System.out.println("Program finished");
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                System.err.println("Error in close Threads: " + e.getMessage());
            }
        }));
    }
    

    public static void main(String[] args) throws InterruptedException 
    {
       
            System.out.println("\n*********************************************");
            System.out.println("Starting Docker Bluetooth Gateway module...");
            System.out.println("*********************************************");

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