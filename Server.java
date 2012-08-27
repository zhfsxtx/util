package hh.qtom.Server;

import hh.qtom.util.MyLogger;

import java.net.InetSocketAddress;
import java.util.concurrent.Executors;

import org.jboss.netty.bootstrap.ServerBootstrap;
import org.jboss.netty.channel.Channel;
import org.jboss.netty.channel.ChannelFactory;
import org.jboss.netty.channel.ChannelPipeline;
import org.jboss.netty.channel.ChannelPipelineFactory;
import org.jboss.netty.channel.Channels;
import org.jboss.netty.channel.group.ChannelGroup;
import org.jboss.netty.channel.group.ChannelGroupFuture;
import org.jboss.netty.channel.group.DefaultChannelGroup;
import org.jboss.netty.channel.socket.nio.NioServerSocketChannelFactory;

public class Server implements Runnable{
	
	private   volatile   static  Server server = null;
	private ChannelGroup group = new DefaultChannelGroup("server-group");
	private MyLogger myLogger = MyLogger.getLogger(Server.class.getName());
	private final Object _lock = new Object();
	
	public   static  Server getIntance() {  
	    if  (server ==  null ) {
	        synchronized  (Server.class) {
		        if  (server ==  null ) {
		        	server = new  Server(); 
		        }
	        }
	    }
	    return  server;  
	}  
	
	public ChannelGroup getGroup() {
		return group;
	}

	@Override
	public void run(){

		ChannelFactory factory = new NioServerSocketChannelFactory(
		        Executors.newCachedThreadPool(),
		        Executors.newCachedThreadPool());
		
		ServerBootstrap bootstrap = new ServerBootstrap(factory);
		bootstrap.setPipelineFactory(new MessageServerPipelineFactory());
		
		bootstrap.setOption("child.tcpNoDelay", true);
		bootstrap.setOption("child.keepAlive", true);
		Channel ch = bootstrap.bind(new InetSocketAddress(8080));
		myLogger.d("Server bind on port 8080 sucessfully\n");
		
		group.add(ch);
		waitShutdown();
		ChannelGroupFuture future = group.close();
		future.awaitUninterruptibly();
		factory.releaseExternalResources();
		
		
	}
	
	
	private void waitShutdown(){
		synchronized (_lock) {
			try {
				_lock.wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	
	public void shutdown(){
		synchronized (_lock) {
			_lock.notifyAll();
			myLogger.d("Server shutdown\n");
		}
	}
	
	public void start(){
		synchronized (_lock) {
			new Thread(Server.getIntance()).start();
			myLogger.d("Server start\n");
		}
		
	}
}
