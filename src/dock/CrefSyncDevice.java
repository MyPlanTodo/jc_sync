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

package dock;

import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.math.BigInteger;

import com.sun.javacard.apduio.Apdu;
import com.sun.javacard.apduio.CadDevice;
import com.sun.javacard.apduio.CadClientInterface;
import com.sun.javacard.apduio.CadTransportException;

/**
 * This class implements an sync device using cref as the underlining device.
 */
public class CrefSyncDevice implements SyncDevice {

    /**
     * The installer applet AID in cref.
     */
    private static final byte[] INSTALLER_AID = {(byte)0xa0, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x62, (byte)0x03, (byte)0x01, (byte)0x08, (byte)0x01};

    /**
     * The current port to use if a new cref should be launched.
     */
    private static int currentPort = 9025;

    private int port = 0;

    /**
     * The host name of where to connect to cref.
     */
    private String host = "localhost";

    /**
     * The eeprom image to give to cref.
     */
    private String eeprom = "";

    /**
     * The interface for powering up and down the device as well as exchanging apdu.
     */
    private CadClientInterface device = null;

    /**
     * True if the device is actually connected to cref.
     */
    private boolean isConnected = false;

    /**
     * The AID of the currently selected applet on the device.
     */
    private BigInteger selectedApplet = null;


    private boolean shouldLaunchCREF = false;

    /**
     * Create an sync device using the given eeprom image.
     * @param eeprom A path to an eeprom image.
     */
    public CrefSyncDevice(String eeprom) {

        this.eeprom = eeprom;
        shouldLaunchCREF = true;
        port = currentPort;
        currentPort+=2;

    }


    /**
     * Create an sync device and connect it to the cref simulator.
     * @param host The host name of the machine where the cref simulator is running.
     * @param port The port of the machine where the cref simulator is running.
     */
    public CrefSyncDevice(String host, int port) {

        this.port = port;
        this.host = host;

    }


    /**
     * Take two byte to make a short.
     * @param b1 The byte of high order.
     * @param b2 The byte of low order.
     * @return The built short.
     */
    private short makeShort(byte b1, byte b2) {

        return (short)(((b1 & 0xFF) << 8) | (b2 & 0xFF));

    }


    /**
     * Print a status word
     * @param sw The status word to print.
     */
    private void printSW(short sw) {

        short sw1 = (short)((sw >> 8) & 0x00FF);
        short sw2 = (short)(sw & 0x00FF);
        System.err.println("Status word: " + (sw1 < 16 ? "0x0" : "0x") + Integer.toHexString(sw1) + (sw2 < 16 ? " 0x0" : " 0x") + Integer.toHexString(sw2));

    }


    /**
     * Connect the device to cref and power it up.
     * @return Return true if successful, false else.
     */
    public boolean connect() {

        if(isConnected)
            return true;

        if(shouldLaunchCREF) {
            String[] cmd = {"/bin/bash", "-c", "cref -i " + eeprom + " -o " + eeprom + " -p " + port + " 1>/dev/null"};
            Process cref = null;
            System.out.println("Launching an instance of cref...");

            try {
                cref = Runtime.getRuntime().exec(cmd);
            }
            catch(IOException e) {
                System.err.println("Cannot launch cref: "+e.getMessage());
                return false;
            }

            try {
                Thread.currentThread().sleep(1000);
            } catch(InterruptedException e) {
            }

            boolean isRunning = false;

            try {
                cref.exitValue();
            }
            catch(IllegalThreadStateException e) {
                isRunning = true;
            }

            if(!isRunning) {
                System.err.println("Cannot launch cref");
                return false;
            }
        }

        Socket socket = null;
        try {
            socket = new Socket(host, port);
        }
        catch(UnknownHostException e) {
            System.err.println("Cannot connect to cref: "+e.getMessage());
            return false;
        }
        catch(IOException e) {
            System.err.println("Cannot connect to cref: "+e.getMessage());
            return false;
        }

        try {
            device = CadDevice.getCadClientInstance(CadDevice.PROTOCOL_T1, socket.getInputStream(), socket.getOutputStream());
        }
        catch(IOException e) {
            System.err.println("Cannot create an Sync device");
            return false;
        }

        isConnected = true;

        try {
            System.out.println("Powering up the sync device");
            device.powerUp();
        }
        catch(IOException e) {
            System.err.println(e.getMessage());
            return false;
        }
        catch(CadTransportException e) {
            System.err.println(e.getMessage());
            return false;
        }

        return true;

    }


    /**
     * Power the device down and thus disconnect it from cref.
     * @return Return true if successful, false else.
     */
    public boolean disconnect() {

        if(!isConnected)
            return true;

        try {
            System.out.println("Powering down the sync device");
            device.powerDown();
        }
        catch(IOException e) {
            System.err.println(e.getMessage());
        }
        catch(CadTransportException e) {
            System.err.println(e.getMessage());
        }

        selectedApplet = null;
        isConnected = false;
        device = null;

        return true;

    }


    /**
     * Select an applet given its AID from this sync device.
     * @param appletAID The AID of applet to select.
     * @return Return true if successful, false else.
     */
    public boolean selectAppletByAID(byte[] appletAID)
        throws AppletNotFoundException {

        if(!isConnected) {
            System.err.println("The cref sync device is not connected");
            return false;
        }

        BigInteger aid = new BigInteger(appletAID);
/*        if((selectedApplet != null) && (selectedApplet.equals(aid)))
            return true;
*/
        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x00;
        apdu.command[Apdu.INS] = (byte)0xA4;
        apdu.command[Apdu.P1]  = (byte)0x04;
        apdu.command[Apdu.P2]  = (byte)0x00;
        apdu.setDataIn(appletAID, appletAID.length);
        apdu.setLe(0);

        if(!exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw == (short)0x6D00)
            throw new AppletNotFoundException();

        if(sw == (short)0x9000) {
            selectedApplet = aid;
            return true;
        }

        printSW(sw);
        selectedApplet = null;
        return false;

    }


    /**
     * Send an apdu to this sync device.
     * @param apdu The apdu to send.
     * @return Return true if successful, false else.
     */
    public boolean exchangeApdu(Apdu apdu) {

        if(!isConnected) {
            System.err.println("The cref sync device is not connected");
            return false;
        }

        try{
            device.exchangeApdu(apdu);
        }
        catch(IOException e) {
            System.err.println(e.getMessage());
            return false;
        }
        catch(CadTransportException e) {
            System.err.println(e.getMessage());
            return false;
        }

//        System.out.println(apdu);

        return true;

    }


    /**
     * Upload a CAP file to this sync device given an InputStream to read from.
     * @param CAPFile An InputStream to read the CAP file from.
     * @param CAPFileSize The size in byte of the CAP file.
     * @return Return true if successful, false else.
     */
    public boolean uploadAppletCAPFile(InputStream CAPFile, long CAPFileSize) {

        System.out.println("Uploading...");

        if(!isConnected) {
            System.err.println("The cref sync device is not connected");
            return false;
        }
   
        CrefInstaller installer = null;
        
        try { 
            installer = new CrefInstaller(CAPFile);
        }
        catch(FileNotFoundException e) {
            System.err.println(e.getMessage());
            return false;
        }
        catch(IOException e) {
            System.err.println(e.getMessage());
            return false;
        }

        if(!installer.processCAPFile())
            return false;

        try {
            if(!selectAppletByAID(INSTALLER_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            System.err.println("Cref installer applet is not found");
            return false;
        }

        if(!installer.uploadCAP(device))
            return false;

        return true;

    }


    /**
     * Create an instance of an applet given its AID on this sync device.
     * @param appletAID The AID of the applet.
     * @return Return true if successful, false else.
     */
    public boolean createAppletInstance(byte[] appletAID) {

        System.out.println("Instantiating...");

        try {
            if(!selectAppletByAID(INSTALLER_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            System.err.println("Cref installer applet is not found");
            return false;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)0xb8;
        apdu.command[Apdu.P1] = (byte)0x00;
        apdu.command[Apdu.P2] = (byte)0x00;
        apdu.setLc(appletAID.length + 1);
        apdu.dataIn[0] = (byte) (appletAID.length & 0xFF);
        for(int i = 0; i < appletAID.length; i++)
            apdu.dataIn[i + 1] = appletAID[i];
        apdu.setLe(0);

        if(!exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        return true;

    }


    /**
     * Delete an applet given its AID on this sync device.
     * @param appletAID The AID of the applet to delete.
     * @return Return true if successful, false else.
     */
    public boolean deleteAppletByAID(byte[] appletAID) {

        try {
            if(!selectAppletByAID(INSTALLER_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            System.err.println("Cref installer applet is not found");
            return false;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)0xC4;
        apdu.command[Apdu.P1]  = (byte)0x01;
        apdu.command[Apdu.P2]  = (byte)0x00;
        apdu.setLc(appletAID.length + 1);
        apdu.dataIn[0] = (byte)appletAID.length;
        for(int i = 0; i < appletAID.length; i++)
            apdu.dataIn[i + 1] = appletAID[i];
        apdu.setLe(0);

        if(!exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        return true;

    }


    /**
     * Delete a package given its AID.
     * @param packageAID The AID of the package to delete.
     * @return Return true if successful, false else.
     */
    public boolean deletePackageByAID(byte[] packageAID) {

        try {
            if(!selectAppletByAID(INSTALLER_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            System.err.println("Cref installer applet is not found");
            return false;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)0xC0;
        apdu.command[Apdu.P1]  = (byte)0x00;
        apdu.command[Apdu.P2]  = (byte)0x00;
        apdu.setLc(packageAID.length + 1);
        apdu.dataIn[0] = (byte)packageAID.length;
        for(int i = 0; i < packageAID.length; i++)
            apdu.dataIn[i + 1] = packageAID[i];
        apdu.setLe(0);

        if(!exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        return true;

    }


    /**
     * Delete a package and its applets given the package AID.
     * @param packageAID The AID of the package to delete.
     * @return Return true if successful, false else.
     */
    public boolean deletePackageAndAppletsByAID(byte[] packageAID) {

        try {
            if(!selectAppletByAID(INSTALLER_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            System.err.println("Cref installer applet is not found");
            return false;
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)0xC2;
        apdu.command[Apdu.P1]  = (byte)0x00;
        apdu.command[Apdu.P2]  = (byte)0x00;
        apdu.setLc(packageAID.length + 1);
        apdu.dataIn[0] = (byte)packageAID.length;
        for(int i = 0; i < packageAID.length; i++)
            apdu.dataIn[i + 1] = packageAID[i];
        apdu.setLe(0);

        if(!exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        return true;

    }

}
