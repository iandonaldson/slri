import org.blueprint.seqhound.*;
import org.blueprint.seqhound.formbeans.*;
import org.blueprint.seqhound.jseqrem.*;
import org.blueprint.bind.framework.formbeans.FormBean;
import org.blueprint.bind.framework.ui.SelectOption;
import org.blueprint.bind.framework.ui.SelectOption.OptionList;
import org.blueprint.bind.framework.ui.Button;
import java.util.*;





public class mainTest
{

  public static String qTime="dfff";

  public mainTest() {
  }

  public static void main(String[] args) {

    //accessTimeControl atc = new accessTimeControl();
    //atc.setNow();
    mainTest mainTest1 = new mainTest();
    mainTest1.testStart();
   // atc.setOut();
   // System.out.println("system delay == " +  atc.parseTime(atc.dateAccount()));

  }

  public void testStart()
  {

    SeqHoundSearchFormBean ssfb = new SeqHoundSearchFormBean();

    System.out.println("ready to start");

   try {
     Button bo = ssfb.getButton("defline", "bbs/buttons/", "Search SeqHound", "validate",
                   new String[] {"document.quick","document.quick.input", "document.quick.type"});

      if (null != bo) {
           System.out.println(Integer.toString(bo.getPg())+"|"+"|"+"|"+bo.getFunctionName()+"|" +bo.getImagePath()+"|" +bo.getFormName() + " | " +bo.getDescription() + " | " +bo.getArguments() );
         }

   }
   catch (Exception e){
     e.printStackTrace();
   }
   finally{
    System.out.println("ok test finished");
   }



  };


}
