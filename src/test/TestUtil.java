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

import com.sun.javacard.apduio.Apdu;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.IOException;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Random;
import java.util.Date;

import dock.AppletNotFoundException;
import dock.SyncDevice;
import dock.Dock;

import synclib.SyncINS;

public class TestUtil {

    private static final byte[] rawUserKey = {(byte)0x49, (byte)0x8C, (byte)0xEB, (byte)0xA5, (byte)0x43, (byte)0x2A, (byte)0x0C, (byte)0x08, (byte)0x80, (byte)0x53, (byte)0x25, (byte)0xF4, (byte)0x94, (byte)0xE0, (byte)0x62, (byte)0x75};

    private static final byte[] userIV = {(byte)0xF4, (byte)0x21, (byte)0x2C, (byte)0xFD, (byte)0x3A, (byte)0x86, (byte)0x2C, (byte)0x1E, (byte)0x59, (byte)0x2F, (byte)0x16, (byte)0x16, (byte)0x14, (byte)0x1F, (byte)0x49, (byte)0xE6};


    public static short makeShort(byte b1, byte b2) {

        return (short)(((b1 & 0xFF) << 8) | (b2 & 0xFF));

    }


    public static void printSW(short sw) {

        short sw1 = (short)((sw >> 8) & 0x00FF);
        short sw2 = (short)(sw & 0x00FF);
        System.err.println("Status word: " + (sw1 < 16 ? "0x0" : "0x") + Integer.toHexString(sw1) + (sw2 < 16 ? " 0x0" : " 0x") + Integer.toHexString(sw2));

    }


    public static void cleaning(BufferedReader in, PrintWriter out, Socket socket) {

        if(in != null)
            try {
                in.close();
            }
            catch(IOException e) {
            }

        if(out != null)
            out.close();

        if(socket != null)
            try {
                socket.close();
            }
            catch(IOException e) {
            }

    }


    public static boolean bindDock(String dockId, String host, int port){

        Socket socketToBridge = null;
        try {
            socketToBridge = new Socket("localhost", 4242);
        }
        catch(IOException e) {
            System.err.println(e.getMessage());
            return false;
        }

        PrintWriter out = null;

        try {
            out = new PrintWriter(socketToBridge.getOutputStream());
        }
        catch(IOException e) {
            System.err.println(e.getMessage());

            cleaning(null, null, socketToBridge);

            return false;
        }

        out.println("ADD BINDING");
        out.println(dockId);
        out.println("localhost");
        out.println(port);
        out.flush();

        cleaning(null, out, socketToBridge);

        return true;

    }


    public static boolean installUserkey(SyncDevice device) {

        try{
            if(!device.selectAppletByAID(Dock.sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            return false;
        }

        byte[] payload = new byte[rawUserKey.length + userIV.length];
        for(int i = 0; i < rawUserKey.length; i++)
            payload[i] = rawUserKey[i];

        for(int i = 0; i < userIV.length; i++)
            payload[rawUserKey.length + i] = userIV[i];

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.DUMMY_INSTALL_USERKEY;
        apdu.command[Apdu.P1] = (byte)0x00;
        apdu.command[Apdu.P2] = (byte)0x00;
        apdu.setDataIn(payload);
        apdu.setLe(0);

        if(!device.exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        return true;

    }


    public static boolean setSeed(SyncDevice device) {

        try{
            if(!device.selectAppletByAID(Dock.sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            return false;
        }

        byte[] seed = new byte[32];
        Random rand = new Random((new Date()).getTime());
        rand.nextBytes(seed);

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.DUMMY_SET_SEED;
        apdu.command[Apdu.P1] = (byte)0x00;
        apdu.command[Apdu.P2] = (byte)0x00;
        apdu.setDataIn(seed);
        apdu.setLe(0);

        if(!device.exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        return true;

    }


    public static short getTestAppletArraySize(SyncDevice device, byte[] AID, int arrayId) {

        try {
            if(!device.selectAppletByAID(AID))
                return -1;
        }
        catch(AppletNotFoundException e) {
            return -1;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)(0x20 | (arrayId & 0x0F));
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setLc(0);
        apdu.setLe(2);

        if(!device.exchangeApdu(apdu))
            return -1;

        byte[] data = apdu.getDataOut();
        return makeShort(data[0], data[1]);

    }


    public static boolean setTestAppletArraySize(SyncDevice device, byte[] AID, int arrayId, int arraySize) {

        try {
            if(!device.selectAppletByAID(AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            return false;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)(0x30 | (arrayId & 0x0F));
        apdu.command[Apdu.P1] = (byte)(arraySize & 0xFF);
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setLc(0);
        apdu.setLe(0);

        return device.exchangeApdu(apdu);

    }


    public static byte getTestAppletArrayValue(SyncDevice device, byte[] AID, int arrayId, int valueId) {

        try {
            if(!device.selectAppletByAID(AID))
                return 0;
        }
        catch(AppletNotFoundException e) {
            return 0;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)(0x10 | (arrayId & 0x0F));
        apdu.command[Apdu.P1] = (byte)(valueId & 0xFF);
        apdu.command[Apdu.P2] = (byte)0x00;
        apdu.setLc(0);
        apdu.setLe(1);

        if(!device.exchangeApdu(apdu))
            return 0;

        return apdu.getDataOut()[0];
 
    }

 
    public static boolean setTestAppletArrayValue(SyncDevice device, byte[] AID, int arrayId, int valueId, byte value) {

        try {
            if(!device.selectAppletByAID(AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            return false;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)(arrayId & 0x0F);
        apdu.command[Apdu.P1] = (byte)(valueId & 0xFF);
        apdu.command[Apdu.P2] = value;
        apdu.setLc(0);
        apdu.setLe(0);

        return device.exchangeApdu(apdu);

    }


    public static boolean changeTestAppletArrayValues(SyncDevice device, byte[] AID, boolean hasClassField) {

        Random gen = new Random();

        try {
            if(!device.selectAppletByAID(AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            return false;
        }

        if(!setTestAppletField(device, AID, (byte)3, (short)((gen.nextInt() % 2) != 0 ? 1 : 0)))
            return false;

        if(!setTestAppletField(device, AID, (byte)4, (short)((gen.nextInt() % 2) != 0 ? 1 : 0)))
            return false;

        if(!setTestAppletField(device, AID, (byte)5, (short)(gen.nextInt() & 0xFF)))
            return false;

        if(!setTestAppletField(device, AID, (byte)6, (short)(gen.nextInt() & 0xFFFF)))
            return false;

        short booleanArraySize = getTestAppletArraySize(device, AID, 0);
        short bArraySize = getTestAppletArraySize(device, AID, 1);
        short sArraySize = getTestAppletArraySize(device, AID, 2);

        for(short i = (short)0; i < booleanArraySize; i++)
            if(!setTestAppletArrayValue(device, AID, 0, i, (byte)((gen.nextInt() % 2) != 0 ? 1 : 0)))
                return false;

        for(short i = (short)0; i < bArraySize; i++)
            if(!setTestAppletArrayValue(device, AID, 1, i, (byte)(gen.nextInt() & 0xFF)))
                return false;

        for(short i = (short)0; i < sArraySize; i++)
            if(!setTestAppletArrayValue(device, AID, 2, i, (byte)(gen.nextInt() & 0xFF)))
                return false;

        if(hasClassField) {
            if(!setTestAppletField(device, AID, (byte)7, (short)(gen.nextInt() & 0xFFFF)))
                return false;
        }

        System.out.println("Applet data tweaked");

        return true;

    }


    public static boolean[] getTestAppletBooleanArray(SyncDevice device, byte[] AID) {

        try {
            if(!device.selectAppletByAID(AID))
                return null;
        }
        catch(AppletNotFoundException e) {
            return null;
        }

        int arraySize = getTestAppletArraySize(device, AID, 0);
        boolean[] newArray = new boolean[arraySize];
        for(int i = 0; i < arraySize; i++)
            newArray[i] = getTestAppletArrayValue(device, AID, 0, i) == 1 ? true : false;

        return newArray;

    }


    public static byte[] getTestAppletByteArray(SyncDevice device, byte[] AID) {

        try {
            if(!device.selectAppletByAID(AID))
                return null;
        }
        catch(AppletNotFoundException e) {
            return null;
        }

        int arraySize = getTestAppletArraySize(device, AID, 1);
        byte[] newArray = new byte[arraySize];
        for(int i = 0; i < arraySize; i++)
            newArray[i] = getTestAppletArrayValue(device, AID, 1, i);

        return newArray;

    }


    public static short[] getTestAppletShortArray(SyncDevice device, byte[] AID) {

        try {
            if(!device.selectAppletByAID(AID))
                return null;
        }
        catch(AppletNotFoundException e) {
            return null;
        }

        int arraySize = getTestAppletArraySize(device, AID, 2);
        short[] newArray = new short[arraySize];
        for(int i = 0; i < arraySize; i++)
            newArray[i] = (short)(getTestAppletArrayValue(device, AID, 2, i) & 0x00FF);

        return newArray;

    }


    public static boolean setTestAppletField(SyncDevice device, byte[] AID, byte field, short value) {

        try {
            if(!device.selectAppletByAID(AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            return false;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)(field & 0x0F);
        apdu.command[Apdu.P1] = (byte)((value >> 8) & 0xFF);
        apdu.command[Apdu.P2] = (byte)(value & 0xFF);
        apdu.setLc(0);
        apdu.setLe(0);

        return device.exchangeApdu(apdu);

    }


    public static byte getTestAppletByteField(SyncDevice device, byte[] AID, byte field) {

        try {
            if(!device.selectAppletByAID(AID))
                return 0;
        }
        catch(AppletNotFoundException e) {
            return 0;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)(0x10 | (field & 0x0F));
        apdu.command[Apdu.P1] = (byte)0x00;
        apdu.command[Apdu.P2] = (byte)0x00;
        apdu.setLc(0);
        apdu.setLe(1);

        if(!device.exchangeApdu(apdu))
            return 0;

        return apdu.getDataOut()[0];

    }


    public static short getTestAppletShortField(SyncDevice device, byte[] AID, byte field) {

        try {
            if(!device.selectAppletByAID(AID))
                return 0;
        }
        catch(AppletNotFoundException e) {
            return 0;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)(0x10 | (field & 0x0F));
        apdu.command[Apdu.P1] = (byte)0x00;
        apdu.command[Apdu.P2] = (byte)0x00;
        apdu.setLc(0);
        apdu.setLe(2);

        if(!device.exchangeApdu(apdu))
            return 0;

        return (short)((apdu.getDataOut()[0] << 8) | apdu.getDataOut()[1]);

    }


    public static boolean have_same_data(SyncDevice device1, SyncDevice device2, byte[] AID, boolean hasClassField) {

        boolean bTrue1 = getTestAppletByteField(device1, AID, (byte)3) == 0 ? false : true;
        boolean bFalse1 = getTestAppletByteField(device1, AID, (byte)4) == 0 ? false : true;
        byte b1 = getTestAppletByteField(device1, AID, (byte)5);
        short s1 = getTestAppletShortField(device1, AID, (byte)6);
        boolean[] booleanArray1 = getTestAppletBooleanArray(device1, AID);
        byte[] bArray1 = getTestAppletByteArray(device1, AID);
        short[] sArray1 = getTestAppletShortArray(device1, AID);

        boolean bTrue2 = getTestAppletByteField(device2, AID, (byte)3) == 0 ? false : true;
        boolean bFalse2 = getTestAppletByteField(device2, AID, (byte)4) == 0 ? false : true;
        byte b2 = getTestAppletByteField(device2, AID, (byte)5);
        short s2 = getTestAppletShortField(device2, AID, (byte)6);
        boolean[] booleanArray2 = getTestAppletBooleanArray(device2, AID);
        byte[] bArray2 = getTestAppletByteArray(device2, AID);
        short[] sArray2 = getTestAppletShortArray(device2, AID);

        if(bTrue1 != bTrue2) {
            System.err.println("bTrue field values are not the same");
            return false;
        }

        if(bFalse1 != bFalse2) {
            System.err.println("bFalse field values are not the same");
            return false;
        }

        if(b1 != b2) {
            System.err.println("b field values are not the same");
            return false;
        }

        if(s1 != s2) {
            System.err.println("s field values are not the same");
            return false;
        }

        if(booleanArray1.length != booleanArray2.length) {
            System.err.println("boolean array lengths are not the same");
            return false;
        }

        if(bArray1.length != bArray2.length) {
            System.err.println("byte array lengths are not the same");
            return false;
        }

        if(sArray1.length != sArray2.length) {
            System.err.println("short array lengths are not the same");
            return false;
        }

        for(int i = 0; i < booleanArray1.length; ++i)
            if(booleanArray1[i] != booleanArray2[i]) {
                System.err.println("booleanArray["+i+"] values are not the same");
                return false;
            }

        for(int i = 0; i < bArray1.length; ++i)
            if(bArray1[i] != bArray2[i]) {
                System.err.println("bArray["+i+"] values are not the same");
                return false;
            }

        for(int i = 0; i < sArray1.length; ++i)
            if(sArray1[i] != sArray2[i]) {
                System.err.println("sArray["+i+"] values are not the same");
                return false;
            }

        if(hasClassField) {
            short classShort1 = getTestAppletShortField(device1, AID, (byte)7);
            short classShort2 = getTestAppletShortField(device2, AID, (byte)7);
            if(classShort1 != classShort2)
                return false;
        }

        return true;

    }


    public static String copyEeprom(String file)
        throws Exception {

        File tmp = File.createTempFile("temp", "img");
        FileInputStream inputFile = new FileInputStream(file);
        FileOutputStream outputFile = new FileOutputStream(tmp);
        byte[] buffer = new byte[1024];
        int nbRead = -1;

        while((nbRead = inputFile.read(buffer)) != -1)
            outputFile.write(buffer, 0, nbRead);
        inputFile.close();

        return tmp.getAbsolutePath();

    }


    public static String dataToString(ArrayList<byte[]> serializedData) {

        StringBuffer stringHex = new StringBuffer();
        Iterator<byte[]> it = serializedData.iterator();

        while(it.hasNext()) {
            byte[] data = it.next();

            for(int j = 0; j < data.length; j++) {
                String hex = Integer.toHexString(0xFF & data[j]).toUpperCase();
                if(hex.length() == 1)
                    stringHex.append("0x0");
                else
                    stringHex.append("0x");
                stringHex.append(hex);
                if(j < (data.length - 1))
                    stringHex.append(",");
            }

            if(it.hasNext())
                stringHex.append("\n");
        }

        return stringHex.toString();

    }

}
