/*
 * Updated on June 4, 2003
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */

import java.io.*;
/**
 * @author kzheng
 *
 * pick a random subset of size n from the original set of size of m
 * syntax :
 *  % java Randompick n m input_file output_file. 
 * 
 */
public class RandomPick {
   
    static void sort(int a[], int lo0, int hi0) {
     int lo = lo0;
     int hi = hi0;
     if (lo >= hi) {
         return;
     }
     int mid = a[(lo + hi) / 2];
     while (lo < hi) {
         while (lo<hi && a[lo] < mid) {
             lo++;
         }
         while (lo<hi && a[hi] > mid) {
             hi--;
         }
         if (lo < hi) {
             int T = a[lo];
             a[lo] = a[hi];
             a[hi] = T;
         }
     }
     if (hi < lo) {
         int T = hi;
         hi = lo;
         lo = T;
     }
     sort(a, lo0, lo);
     sort(a, lo == lo0 ? lo+1 : lo, hi0);
    }

    static void sort(int a[]) {
         sort(a, 0, a.length-1);
    }

    public static void main(String[] args) {    
        // read command line arguments
        String usage = "Usage: java num_pick num_total infile outfile";
        if(args.length<4){
         System.out.println(usage); 
         System.exit(1);
        }
        
        int n= Integer.valueOf(args[0]).intValue();
        int m = Integer.valueOf(args[1]).intValue();
        String infile=args[2];
        String outfile=args[3];
		

		// open files
        FileWriter fileWriter=null;
        PrintWriter out=null;
        try{    fileWriter = new FileWriter(outfile);}
        catch(IOException ioe){
            System.out.println(ioe.toString());
            System.exit(1);
        }
        out = new PrintWriter(fileWriter);
               
        int random[] = new int[n];
        for(int i = 0; i<n; i++){
            while(true){
                int tmp= (int)(Math.random() *m);
                boolean duplicated = false;
                for(int j=0; j<i; j++){
                    if(tmp == random[j]){
                        duplicated = true;
                        break;
                    }               
                }
                if(!duplicated) {
                    random[i] = tmp;
                    break;
                }
            }
        }
        sort(random);
        for(int i=0; i<n; i++){
            System.out.println(""+random[i]);
        }
        //out.println("hello World !");
        //try{fileWriter.close();}
        //catch(IOException ioe){ System.out.println(ioe.toString());}
        //System.exit(1);            
        
        //outer loop: sent file
        int i =0;
        int total = 0;
        BufferedReader br1=null;
        try{ br1 = new BufferedReader(new FileReader(infile));
        } catch (FileNotFoundException e)
        {    System.out.println("File not found: " + infile);
            System.exit(1);
        }
        while(i<n && total<m){
            String line=null;
            try{
              line = br1.readLine();
            }catch (IOException e){
              System.out.println("Error reading sent file");    
              System.exit(1);
            }
            if(line==null) break;
            if(line.trim().length() <2) continue;
            if (total==random[i]){
              out.println(line);
              i++;
            }
            total++;    
        }
        
        //closefiles
        try{fileWriter.close();}
        catch(IOException ioe){ System.out.println(ioe.toString());}
    }
}
