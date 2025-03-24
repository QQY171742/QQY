package ioTest2;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;


public class FileReaderDemo {

	public static void main(String[] args) {
		String path = "D:\\c\\test\\test1.txt";
		String result = fileRead(path);
		System.out.println(result);
	}
	/**
	 * ��
	 * @param filePath
	 * @return
	 */
	public static String fileRead(String filePath){
		StringBuilder result = new StringBuilder();
		File src = new File(filePath);
		FileReader fread = null;
		BufferedReader re = null;
		try {
			fread = new FileReader(src);
			re = new BufferedReader(fread);
			while(true){
				String  str = re.readLine();
				if(str == null){
					break;
				}
				result.append(str);
				result.append("\n");
			}
			
		} catch (IOException e) {
			e.printStackTrace();
		}finally{
			try {
				re.close();
				fread.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return result.toString();
		
	}
}
