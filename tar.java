package com.bbk.PCTools.util;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import android.util.Log;

import com.bbk.PCTools.tar.TarEntry;
import com.bbk.PCTools.tar.TarInputStream;
import com.bbk.PCTools.tar.TarOutputStream;


public class tar {
	private static final String TAG = "TAR";
	/***
	 * 
	 * @param src   Directory that to be tar
	 * @param dest  Dest file what to tar for
	 * @throws IOException
	 */
	public static void tarDir(String src, String dest) throws IOException {

		FileOutputStream destFile = new FileOutputStream(dest);
		TarOutputStream out = new TarOutputStream(new BufferedOutputStream(
				destFile));

		Log.d(TAG, "tarDir(" + "srcDir:" + src + ",desFile:" + dest + ")");

		tarDir(out, src, "");
		
		out.close();
		
		Log.d(TAG, "tarDir(" + "srcDir:" + src + ",desFile:" + dest + ") success !!!");
	}
	
	/***
	 * 
	 * @param src  File that to be tar
	 * @param dest est file what to tar for
	 * @throws IOException
	 */

	public static void tarFile(String src, String dest) throws IOException {
		FileOutputStream destFile = new FileOutputStream(dest);
		TarOutputStream out = new TarOutputStream(new BufferedOutputStream(
				destFile));
		Log.d(TAG, "tarFile(" + "srcFile:" + src + ",desFile:" + dest + ")");
		
		File f = new File(src);
		tarFile(out, f, f.getName());
		out.flush();
		out.close();
		
		Log.d(TAG, "tarFile(" + "srcFile:" + src + ",desFile:" + dest + ") success !!!");
	}
	
	
	/***
	 * 
	 * @param tarFile  tarFile that what to be untar
	 * @param destDir  Dest Directory
	 * @throws IOException
	 */
	public static void untarFile(String tarFile, String destDir) throws IOException {
		
		Log.d(TAG, "untarFile(" + "tarFile:" + tarFile + ",destDir:" + destDir + ")");
		
		File dDir = new File(destDir);
		if(!dDir.exists() || !dDir.isDirectory()){
			if(!dDir.mkdir() && !dDir.mkdirs())
			{
				Log.e(TAG, "make destDir error: " + destDir);
				return;
			}
		}
		TarInputStream tis = new TarInputStream(new BufferedInputStream(new FileInputStream(tarFile)));
		  TarEntry entry;
		  
		  while((entry = tis.getNextEntry()) != null) {
		     int count;
		     byte data[] = new byte[2048];
		     
		     File file = new File(destDir + File.separator + entry.getName());	     
		     if(!file.getParentFile().exists())
		     {
		    	 file.getParentFile().mkdirs();
		     }
		     
		     FileOutputStream fos = new FileOutputStream(file);
		     BufferedOutputStream dest = new BufferedOutputStream(fos);
		  
		     while((count = tis.read(data)) != -1) {
		        dest.write(data, 0, count);
		     }
		  
		     dest.flush();
		     dest.close();
		  }
		  
		  tis.close();
		  
		  Log.d(TAG, "untarFile(" + "tarFile:" + tarFile + ",destDir:" + destDir + ") success!!!");
	}

	private static void tarDir(TarOutputStream out, String src, String dest)
			throws IOException {

		File dir = new File(src);

		if (dir.isDirectory()) {
			File[] fileArray = dir.listFiles();
			if (fileArray != null) {
				for (File file : fileArray) {
					tarDir(out, src + File.separator + file.getName(), dest
							+ File.separator + file.getName());
				}
			} else {
				Log.d(TAG, "addFolder, empty folder:" + src);
			}
		} else {
			tarFile(out, dir, dest);
		}
	}

	
	
	private static void tarFile(TarOutputStream out, File f, String dest)
			throws IOException {
		Log.d(TAG, "src :" + f.getAbsolutePath() + "dest :" + dest);
		out.putNextEntry(new TarEntry(f, dest));
		BufferedInputStream origin = new BufferedInputStream(
				new FileInputStream(f));
		int count;
		byte data[] = new byte[2048];

		while ((count = origin.read(data)) != -1) {
			out.write(data, 0, count);
		}
		
		out.flush();
		origin.close();
	}
}
