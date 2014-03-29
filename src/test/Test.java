/*
 * Copyright Inria:
 * Jean-François Hren
 * 
 * jfhren[at]gmail[dot]com
 * michael[dot]hauspie[at]lifl[dot]com
 * 
 * This software is a computer program whose purpose is to synchronize
 * Java Card 2 applet data.
 * 
 * This software is governed by the CeCILL-B license under French
 * law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the
 * CeCILL-B
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its
 * terms.
 */

package test;

import dock.AppletLibrary;
import dock.LocalAppletLibrary;

public class Test {

    private static final String testApplet1CAPFilePath = "class/test_applet_1/javacard/test_applet_1.cap";
    private static final byte[] testApplet1AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x03, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};

    private static final String testApplet2CAPFilePath = "class/test_applet_2/javacard/test_applet_2.cap";
    private static final byte[] testApplet2AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x04, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};

    private static final String testApplet3CAPFilePath = "class/test_applet_3/javacard/test_applet_3.cap";
    private static final byte[] testApplet3AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x05, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};



    public static void main(String[] args) {

        LocalAppletLibrary library = new LocalAppletLibrary();
        library.add(testApplet1AID, testApplet1CAPFilePath);
        library.add(testApplet2AID, testApplet2CAPFilePath);
        library.add(testApplet3AID, testApplet3CAPFilePath);

        System.out.println(">>>>>>>>>>>> PRINT TESTS <<<<<<<<<<<<");
        if(!TestPrint.start(library, args)) {
            System.out.println(">>>>>>>>>>>>>>>> Print tests failled");
            System.exit(1);
        }

        System.out.println(">>>>>>>>>>>>>>>> Print tests successful");
        System.out.println("\n---------------------------------------\n");


        System.out.println(">>>>>>>>>>>> CHECK TESTS <<<<<<<<<<<<");
        if(!TestCheck.start(library, args)) {
            System.out.println(">>>>>>>>>>>>>>>> Check tests failled");
            System.exit(1);
        }

        System.out.println(">>>>>>>>>>>>>>>> Check tests successful");
        System.out.println("\n---------------------------------------\n");


        System.out.println(">>>>>>>>>>>> UNINSTALL TESTS <<<<<<<<<<<<");
        if(!TestUninstall.start(library, args)) {
            System.out.println(">>>>>>>>>>>>>>>> Uninstall tests failled");
            System.exit(1);
        }

        System.out.println(">>>>>>>>>>>>>>>> Uninstall tests successful");
        System.out.println("\n---------------------------------------\n");


        System.out.println(">>>>>>>>>>>> UPLOAD TESTS <<<<<<<<<<<<");
        if(!TestUpload.start(library, args)) {
            System.out.println(">>>>>>>>>>>>>>>> Upload tests failled");
            System.exit(1);
        }

        System.out.println(">>>>>>>>>>>>>>>> Upload tests successful");
        System.out.println("\n---------------------------------------\n");


        System.out.println(">>>>>>>>>>>> SYNCHRONIZATION TESTS <<<<<<<<<<<<");
        if(!TestSynchronization.start(library, args)) {
            System.out.println(">>>>>>>>>>>>>>>> Synchronization tests failled");
            System.exit(1);
        }

            System.out.println(">>>>>>>>>>>>>>>> Synchronization tests successful");
    }

}
