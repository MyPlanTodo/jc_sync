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

import synclib.SyncSW;
import synclib.SyncINS;

import com.sun.javacard.apduio.Apdu;
import javacard.framework.ISO7816;

import java.util.HashMap;
import java.util.ArrayList;
import java.util.Iterator;
import java.math.BigInteger;
import java.io.IOException;
import java.io.InputStream;

/**
 * This class defines a dock which goal is to synchronize sync devices.
 * The dock contains an internal sync device and should have access to an applet library.
 */
public class Dock {

    /**
     * The AID of the SyncManager applet.
     */
    public static final byte[] sync_AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};

    /**
     * An hashmap pairing an index with each connected sync devices.
     */
    private HashMap<Integer, SyncDevice> list = null;

    /**
     * The number of added devices since the start. Used to generate the index of a newly connected sync device.
     */
    private int nbAddedDevicesSinceStart = 0;

    /**
     * The applet library used when a CAP file is needed.
     */
    private AppletLibrary library;

    /**
     * The internal sync device.
     */
    private SyncDevice internalDevice = null;


    /**
     * Create an instance of the dock.
     * @param library The library used to get CAP files.
     */
    public Dock(AppletLibrary library) {

        list = new HashMap<Integer, SyncDevice>();
        this.library = library;

    }


    /**
     * Create an instance of the dock.
     * @param library The library used to get CAP files.
     * @param internalDevice The sync device installed in the dock.
     */
    public Dock(AppletLibrary library, SyncDevice internalDevice) {

        list = new HashMap<Integer, SyncDevice>();
        this.library = library;
        this.internalDevice = internalDevice;

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
     * Add an sync device to the dock. The device should be already connected.
     * @param device The sync device to add.
     * @return The index of the newly added device.
     */
    public int addSyncDevice(SyncDevice device) {

        list.put(new Integer(nbAddedDevicesSinceStart), device);
        return nbAddedDevicesSinceStart++;

    }


    /**
     * Remove an sync device from the dock given its index. The device is not deconnected.
     * @param deviceIndex The index of the sync device to remove.
     * @return The removed sync device.
     */
    public SyncDevice removeSyncDevice(int deviceIndex) {

        return list.remove(new Integer(deviceIndex));

    }


    /**
     * Select an applet by its AID in an sync device given its index.
     * @param deviceIndex The sync device index in which an applet is selected.
     * @param appletAID The AID of the to-be-selected applet.
     * @return Return true if the select is successful, false else.
     * @throws AppletNotFoundException An AppletNotFoundException is thrown if the requested applet is not found within the device.
     */
    public boolean selectAppletByAID(int deviceIndex, byte[] appletAID)
        throws AppletNotFoundException {

        return list.get(new Integer(deviceIndex)).selectAppletByAID(appletAID);

    }


    /**
     * Select the SyncManager applet in an sync device given its index.
     * @param deviceIndex The sync device index in which the SyncManager applet is selected.
     * @return Return true if the select is successful, false else.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     */
    public boolean selectSyncManager(int deviceIndex)
        throws UnsupportedDeviceException {

        try {
            return list.get(new Integer(deviceIndex)).selectAppletByAID(sync_AID);
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

    }


    /**
     * Print the list of managed AIDs from an sync device given its index.
     * @param deviceIndex The sync device index.
     * @return Return true if everything went fine, false else.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     * @see synclib.SyncINS#LIST_AIDS
     */
    public boolean printManagedAppletsAID(int deviceIndex)
        throws UnsupportedDeviceException {

        SyncDevice device = list.get(new Integer(deviceIndex));

        try {
            if(!device.selectAppletByAID(sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        return printManagedAppletsAID(device);

    }



     /**
     * Print the list of managed AIDs from an sync device.
     * @param device The sync device.
     * @return Return true if everything went fine, false else.
     * @see synclib.SyncINS#LIST_AIDS
     */
    public boolean printManagedAppletsAID(SyncDevice device) {

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)SyncINS.CLA;
        apdu.command[Apdu.INS] = (byte)SyncINS.LIST_AIDS;
        apdu.command[Apdu.P1]  = (byte)0x00;
        apdu.command[Apdu.P2]  = (byte)0x00;
        apdu.setLc(0);
        apdu.setLe(0);

        if(!device.exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println(apdu.toString());

        return true;

    }


    /**
     * Get the number of managed applets in an sync device given its index.
     * @param deviceIndex The sync device index.
     * @return Return the number of managed applets, -1 if an error occurred.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     * @see synclib.SyncINS#GET_NB_AIDS
     */
    public int getNbManagedApplets(int deviceIndex)
        throws UnsupportedDeviceException {

        SyncDevice device = list.get(new Integer(deviceIndex));

        try {
            if(!device.selectAppletByAID(sync_AID))
                return -1;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        return getNbManagedApplets(device);

    }


    /**
     * Get the number of managed applets in an sync device.
     * @param device The sync device.
     * @return Return the number of managed applets, -1 if an error occurred.
     * @see synclib.SyncINS#GET_NB_AIDS
     */
    protected int getNbManagedApplets(SyncDevice device) {

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)SyncINS.CLA;
        apdu.command[Apdu.INS] = (byte)SyncINS.GET_NB_AIDS;
        apdu.command[Apdu.P1]  = (byte)0x00;
        apdu.command[Apdu.P2]  = (byte)0x00;
        apdu.setLc(0);
        apdu.setLe(1);

        if(!device.exchangeApdu(apdu))
            return -1;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return -1;
        }

        if(apdu.getLe() != 1)
            return -1;

        return (int)(apdu.getDataOut()[0]);

    }


    /**
     * Get the AID of an applet given its id in an Sync device given its index.
     * @param deviceIndex The sync device index.
     * @param id The applet id given by the SyncManager of the sync device.
     * @param nbApplets The number of managed applets in the sync device. Used to check if the dock is synced with the device.
     * @return Return the AID of the applet, an empty array if an error occurred.
     * @throws SyncException A SyncException is throws if the given number of managed applets differs from reality.
     * @throws IndexOutOfBoundsException An IndexOutOfBoundsException is thrown if the requested applet's id is out of range.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     * @see synclib.SyncINS#GET_AID_BY_ID
     * @see synclib.SyncSW#WRONG_NB_AIDS
     * @see synclib.SyncSW#WRONG_ID
     */
    public byte[] getAIDById(int deviceIndex, int id, int nbApplets)
        throws SyncException, IndexOutOfBoundsException, UnsupportedDeviceException {

        SyncDevice device = list.get(new Integer(deviceIndex));

        try {
            if(!device.selectAppletByAID(sync_AID))
                return new byte[0];
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        return getAIDById(device, id, nbApplets);

    }


    /**
     * Get the AID of an applet given its id in an Sync device.
     * @param device The sync device.
     * @param id The applet id given by the SyncManager of the sync device.
     * @param nbApplets The number of managed applets in the sync device. Used to check if the dock is synced with the device.
     * @return Return the AID of the applet, an empty array if an error occurred.
     * @throws SyncException A SyncException is throws if the given number of managed applets differs from reality.
     * @throws IndexOutOfBoundsException An IndexOutOfBoundsException is thrown if the requested applet's id is out of range.
     * @see synclib.SyncINS#GET_AID_BY_ID
     * @see synclib.SyncSW#WRONG_NB_AIDS
     * @see synclib.SyncSW#WRONG_ID
     */
    protected byte[] getAIDById(SyncDevice device, int id, int nbApplets)
        throws SyncException, IndexOutOfBoundsException {

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)SyncINS.CLA;
        apdu.command[Apdu.INS] = (byte)SyncINS.GET_AID_BY_ID;
        apdu.command[Apdu.P1]  = (byte)id;
        apdu.command[Apdu.P2]  = (byte)nbApplets;
        apdu.setLc(0);
        apdu.setLe(0);

        if(!device.exchangeApdu(apdu))
            return new byte[0];

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw == SyncSW.WRONG_NB_AIDS)
            throw new SyncException();

        if(sw == SyncSW.WRONG_ID)
            throw new IndexOutOfBoundsException("The requested id is not in range");

        if(sw != (short)0x9000) {
            printSW(sw);
            return new byte[0];
        }

        byte[] data = apdu.getDataOut();
        byte[] newArray = new byte[data.length];
        for(int i = 0; i < data.length; i++)
            newArray[i] = data[i];

        return newArray;

    }


    /**
     * Delete an applet given its AID from an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param appletAID The AID of the applet to delete.
     * @return Return true if successful, false else.
     */
    public boolean deleteAppletByAID(int deviceIndex, byte[] appletAID) {

        return list.get(new Integer(deviceIndex)).deleteAppletByAID(appletAID);

    }


    /**
     * Delete an applet given its id from an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param id The id of the applet to delete.
     * @param nbApplets The number of managed applets within the sync device.
     * @return Return true if successful, false else.
     * @throws SyncException A SyncException is throws if the given number of managed applets differs from reality.
     * @throws IndexOutOfBoundsException An IndexOutOfBoundsException is thrown if the requested applet's id is out of range.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     * @see synclib.SyncSW#WRONG_NB_AIDS
     * @see synclib.SyncSW#WRONG_ID
     */
    public boolean deleteAppletById(int deviceIndex, int id, int nbApplets)
        throws SyncException, IndexOutOfBoundsException, UnsupportedDeviceException {

        SyncDevice device = list.get(new Integer(deviceIndex));
        return deleteAppletById(device, id, nbApplets);

    }


    /**
     * Delete an applet given its id from an sync device.
     * @param device The sync device.
     * @param id The id of the applet to delete.
     * @param nbApplets The number of managed applets within the sync device.
     * @return Return true if successful, false else.
     * @throws SyncException A SyncException is throws if the given number of managed applets differs from reality.
     * @throws IndexOutOfBoundsException An IndexOutOfBoundsException is thrown if the requested applet's id is out of range.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     * @see synclib.SyncSW#WRONG_NB_AIDS
     * @see synclib.SyncSW#WRONG_ID
     */
    protected boolean deleteAppletById(SyncDevice device, int id, int nbApplets)
        throws SyncException, IndexOutOfBoundsException, UnsupportedDeviceException {

        try {
            if(!device.selectAppletByAID(sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        byte[] AID = getAIDById(device, id, nbApplets);
        return device.deleteAppletByAID(AID);

    }


    /**
     * Delete a package given its AID from an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param packageAID The AID of the package to delete.
     * @return Return true if successful, false else.
     */
    public boolean deletePackageByAID(int deviceIndex, byte[] packageAID) {

        return list.get(new Integer(deviceIndex)).deletePackageByAID(packageAID);

    }


    /**
     * Delete a package given its AID from an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param packageAID The AID of the package to delete.
     * @return Return true if successful, false else.
     */
    public boolean deletePackageAndAppletsByAID(int deviceIndex, byte[] packageAID) {

        return list.get(new Integer(deviceIndex)).deletePackageAndAppletsByAID(packageAID);

    }


    /**
     * Check if an applet given its AID is registered within an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param appletAID The AID of the applet to check.
     * @return Return true if the applet is registered within sync, false else.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     * @see synclib.SyncINS#CHECK_INSTALL
     */
    public boolean checkInstallByAID(int deviceIndex, byte[] appletAID)
        throws UnsupportedDeviceException {

        SyncDevice device = list.get(new Integer(deviceIndex));
        try {
            if(!device.selectAppletByAID(sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        try {
            return checkInstallByAID(device, appletAID) != 0;
        }
        catch(AppletNotRegisteredException e) {
            return false;
        }
        catch(AppletNotFoundException e) {
            return false;
        }

    }


    /**
     * Get an applet's id given its AID from an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param appletAID The AID of the applet.
     * @return Return the id of the applet if successful, 0 else.
     * @throws AppletNotRegisteredException An AppletNotRegisteredException is thrown if the requested applet is not registered within sync.
     * @throws AppletNotFoundException An AppletNotFoundException is thrown if the requested applet is not registered with the VM.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     * @see synclib.SyncSW#AID_NOT_FOUND
     * @see synclib.SyncSW#APPLET_NOT_FOUND
     */
    public int getIdByAID(int deviceIndex, byte[] appletAID)
        throws AppletNotRegisteredException, AppletNotFoundException, UnsupportedDeviceException {

        SyncDevice device = list.get(new Integer(deviceIndex));

        try {
            if(!device.selectAppletByAID(sync_AID))
                return 0;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        return checkInstallByAID(device, appletAID);

    }


    /**
     * Check if an applet given its AID is registered within an sync device.
     * @param device The sync device.
     * @param appletAID The AID of the applet.
     * @return Return the id of the applet if successful, 0 else.
     * @throws AppletNotRegisteredException An AppletNotRegisteredException is thrown if the requested applet is not registered within sync.
     * @throws AppletNotFoundException An AppletNotFoundException is thrown if the requested applet is not registered with the VM.
     * @see synclib.SyncSW#AID_NOT_FOUND
     * @see synclib.SyncSW#APPLET_NOT_FOUND
     */
    protected int checkInstallByAID(SyncDevice device, byte[] appletAID)
        throws AppletNotRegisteredException, AppletNotFoundException {

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)SyncINS.CLA;
        apdu.command[Apdu.INS] = (byte)SyncINS.CHECK_INSTALL;
        apdu.command[Apdu.P1]  = (byte)0x00;
        apdu.command[Apdu.P2]  = (byte)0x00;
        apdu.setDataIn(appletAID, appletAID.length);
        apdu.setLe(0);

        if(!device.exchangeApdu(apdu))
            return 0;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw == SyncSW.APPLET_NOT_FOUND)
            throw new AppletNotFoundException();

        if(sw == SyncSW.AID_NOT_FOUND)
            throw new AppletNotRegisteredException();

        if(sw1Sw2[0] != (byte)0x90) {
            printSW(sw);
            return 0;
        }

        return ((int)sw1Sw2[1]) & (int)0xFF;

    }


    /**
     * Upload an applet's package given the applet's AID to an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param appletAID The AID of the applet which package should be uploaded.
     * @return Return true if successful, false else.
     * @throws AppletNotFoundException An AppletNotFoundException is thrown if the package CAP file is not found given the applet's AID in the applet library.
     */
    public boolean uploadAppletByAID(int deviceIndex, byte[] appletAID)
        throws AppletNotFoundException {

        InputStream input = library.getAppletCAPByAID(appletAID);
        long size = library.getAppletCAPSizeByAID(appletAID);
        return list.get(new Integer(deviceIndex)).uploadAppletCAPFile(input, size);

    }


    /**
     * Create an instance of an applet given its AID in an sync device given its index.
     * @param deviceIndex The sync device index.
     * @param appletAID The AID of the applet to instantiate.
     * @return Return true if successful, false else.
     */
    public boolean createAppletInstance(int deviceIndex, byte[] appletAID) {

        return list.get(new Integer(deviceIndex)).createAppletInstance(appletAID);

    }


    /**
     * Send an abort APDU to the given sync device to stop the current challenge process.
     * @param device The device on which the challenge process should be aborted.
     */
    protected void abortChallenge(SyncDevice device) {

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.ABORT_CHALLENGE;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setLc(0);
        apdu.setLe(0);

        device.exchangeApdu(apdu);

    }


    /**
     * Send an APDU to the given sync device to determine if it is configured or not.
     * @param device The device to ask.
     * @return Return true is the sync device is configured, false else.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     */
    public boolean isConfigured(SyncDevice device)
        throws UnsupportedDeviceException {
    
        try {
            if(!device.selectAppletByAID(sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.IS_CONFIGURED;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setLc(0);
        apdu.setLe(0);

        if(!device.exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        return sw == (short)0x9000;

    }


    /**
     * Configuration of a new sync device using the internal sync device as a reference.
     * @param newDevice The device to configure.
     * @return Return true if the configuration process was successful, false else.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException is thrown when the device is not sync compliant.
     */
    protected boolean initNewSyncDevice(SyncDevice newDevice)
        throws UnsupportedDeviceException {

        if(internalDevice == null) {
            System.err.println("There is no internal device");
            return false;
        }

        /* Starting the config process on the new device */

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.START_CONFIG;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setLc(0);
        apdu.setLe(0);

        try {
            if(!newDevice.selectAppletByAID(sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        if(!newDevice.exchangeApdu(apdu))
            return false;

        byte[] sw1Sw2 = apdu.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println("Configuration process started");

        /* Asking a challenge to the internal sync device */

        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.GET_SERVER_CHALLENGE;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setLc(0);
        apdu.setLe(0);

        try {
            if(!internalDevice.selectAppletByAID(sync_AID))
                return false;
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException();
        }

        if(!internalDevice.exchangeApdu(apdu))
            return false;

        sw1Sw2 = apdu.getSw1Sw2();
        sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println("First challenge generated");

        /* We have the challenge from the internal sync device. Let us give it to the new device which will solve it and send its own with the solution */

        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.ANSWER_SERVER_CHALLENGE;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setDataIn(apdu.getDataOut());
        apdu.setLe(0);

        if(!newDevice.exchangeApdu(apdu))
            return false;

        sw1Sw2 = apdu.getSw1Sw2();
        sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println("Second challenge generated");

        /* We have the challenge from the new sync device. Let us give it to the internal sync device which will solve it and send it back */

        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.ANSWER_CLIENT_CHALLENGE;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setDataIn(apdu.getDataOut());
        apdu.setLe(0);

        if(!internalDevice.exchangeApdu(apdu))
            return false;

        sw1Sw2 = apdu.getSw1Sw2();
        sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println("Both challenge answered");

        /* Each device authenticated each other, we now ask the new device for the fingerprint data */

        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.GET_FINGERPRINT;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setDataIn(apdu.getDataOut());
        apdu.setLe(0);

        if(!newDevice.exchangeApdu(apdu))
            return false;

        sw1Sw2 = apdu.getSw1Sw2();
        sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println("Fingerprint data acquired");

        /* We send the fingerprint data from the new device to the internal device which will send the userkey */

        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.GET_USERKEY;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setDataIn(apdu.getDataOut());
        apdu.setLe(0);

        if(!internalDevice.exchangeApdu(apdu))
            return false;

        sw1Sw2 = apdu.getSw1Sw2();
        sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println("Fingerprint data verified, userkey sent");

        /* We send the userkey to the new sync device for installation */

        apdu.command[Apdu.CLA] = SyncINS.CLA;
        apdu.command[Apdu.INS] = SyncINS.INSTALL_USERKEY;
        apdu.command[Apdu.P1] = (byte)0;
        apdu.command[Apdu.P2] = (byte)0;
        apdu.setDataIn(apdu.getDataOut());
        apdu.setLe(0);

        if(!newDevice.exchangeApdu(apdu))
            return false;

        sw1Sw2 = apdu.getSw1Sw2();
        sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        System.out.println("Userkey installed");

        return true;

    }

    /**
     * Synchronize an applet data from two different sync devices.
     * @param source The source sync device from which applet data will be serialized.
     * @param idAppletSource The applet id in the source sync device.
     * @param nbAppletsSource The number of applets within the source sync device.
     * @param destination The destination sync device in which applet data will be merged.
     * @param idAppletDestination The applet id in the destination sync device.
     * @param nbAppletsDestination The number of applets withing the destination sync device.
     * @return Return true if the synchronization was successful, false else.
     */
    protected boolean synchronizeAppletToApplet(SyncDevice source, int idAppletSource, int nbAppletsSource, SyncDevice destination, int idAppletDestination, int nbAppletsDestination) {

        Apdu apduSource = new Apdu();
        Apdu apduDestination = new Apdu();
        short sw = (short)0;

        apduSource.command[Apdu.CLA] = SyncINS.CLA;
        apduSource.command[Apdu.INS] = SyncINS.SERIALIZE_APPLET_BY_ID;
        apduSource.command[Apdu.P1] = (byte)idAppletSource;
        apduSource.command[Apdu.P2] = (byte)nbAppletsSource;
        apduSource.setLc(0);
        apduSource.setLe(0);

        apduDestination.command[Apdu.CLA] = SyncINS.CLA;
        apduDestination.command[Apdu.INS] = SyncINS.MERGE_APPLET_BY_ID;
        apduDestination.command[Apdu.P1] = (byte)idAppletDestination;
        apduDestination.command[Apdu.P2] = (byte)nbAppletsDestination;

        do {

            if(!source.exchangeApdu(apduSource)) {
                System.err.println("Could not send an APDU to the source sync device");
                return false;
            }

            System.out.println("sync_source: " + apduSource);

            byte[] sw1Sw2 = apduSource.getSw1Sw2();
            sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

            if(sw != (short)0x9000)
                break;

            apduDestination.setDataIn(apduSource.getDataOut());
            apduDestination.setLe(0);

            if(!destination.exchangeApdu(apduDestination)) {
                System.err.println("Could not send an APDU to the destination sync device");
                return false;
            }

            System.out.println("sync_destination: " + apduDestination);

            sw1Sw2 = apduDestination.getSw1Sw2();
            sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

            if(sw == SyncSW.MERGE_DONE)
                return true;

            if(sw != (short)0x9000)
                break;

            apduSource.setDataIn(apduDestination.getDataOut());
            apduSource.setLe(0);

        } while(true);

        printSW(sw);
        return false;

    }


    /**
     * Start a synchronization session between two sync devices.
     * @param source The source sync device from which applet data will be serialized.
     * @param destination The destination sync device in which applet data will be merged.
     * @return Return true if the setup of the session was successful, false else.
     */
    protected boolean startSynchronizationSession(SyncDevice source, SyncDevice destination) {

        /* We set up the source device for serialization */

        Apdu apduSource = new Apdu();
        apduSource.command[Apdu.CLA] = SyncINS.CLA;
        apduSource.command[Apdu.INS] = SyncINS.START_SERIALIZE_SESSION;
        apduSource.command[Apdu.P1] = (byte)0;
        apduSource.command[Apdu.P2] = (byte)0;
        apduSource.setLc(0);
        apduSource.setLe(0);

        if(!source.exchangeApdu(apduSource))
            return false;

        /* We set up the destination device for merge and give it the source device parameter */

        Apdu apduDestination = new Apdu();
        apduDestination.command[Apdu.CLA] = SyncINS.CLA;
        apduDestination.command[Apdu.INS] = SyncINS.START_MERGE_SESSION;
        apduDestination.command[Apdu.P1] = (byte)0;
        apduDestination.command[Apdu.P2] = (byte)0;
        apduDestination.setDataIn(apduSource.getDataOut());
        apduDestination.setLe(0);

        if(!destination.exchangeApdu(apduDestination))
            return false;

        byte[] sw1Sw2 = apduSource.getSw1Sw2();
        short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        /* Finally we give to the source device the destination device parameter */

        apduSource.setDataIn(apduDestination.getDataOut());
        apduSource.setLe(0);

        if(!source.exchangeApdu(apduSource))
            return false;

        sw1Sw2 = apduSource.getSw1Sw2();
        sw = makeShort(sw1Sw2[0], sw1Sw2[1]);

        if(sw != (short)0x9000) {
            printSW(sw);
            return false;
        }

        return true;

    }


    /**
     * Synchronize two sync devices in a peer-to-peer fashion, the destination sync device leading the process. Every applets in the source sync device are replicated in the destination sync device.
     * @param source The source sync device from which applets data will be serialized.
     * @param destination The destination sync device in which applets data will be merged.
     * @return Return true if the synchronization was successful, false else.
     * @throws UnsupportedDeviceException An UnsupportedDeviceException exception is thrown if the source or destination device is not sync compliant.
     * @throws SyncException A SyncException exception is thrown if one of the device is not synced with the dock.
     * @throws AppletNotRegisteredException an AppletNotRegisteredException exception is thrown if a newly installed applet was not correctly registered within sync.
     * @throws AppletNotRegisteredException an AppletNotRegisteredException exception is thrown if the CAP file of a missing applet in the destination sync device is not found within the library.
     */
    public boolean synchronizeOneToOne(SyncDevice source, SyncDevice destination)
        throws UnsupportedDeviceException, SyncException, AppletNotRegisteredException, AppletNotFoundException {

        try {
            if(!source.selectAppletByAID(sync_AID)) {
                System.err.println("Could not select the sync manager applet within the source device");
                return false;
            }
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException("The source device is not sync compliant");
        }

        try {
            if(!destination.selectAppletByAID(sync_AID)) {
                System.err.println("Could not select the sync manager applet within the destination device");
                return false;
            }
        }
        catch(AppletNotFoundException e) {
            throw new UnsupportedDeviceException("The destination device is not sync compliant");
        }

        int nbAppletsSource = getNbManagedApplets(source);
        int nbAppletsDestination = getNbManagedApplets(destination);

        System.out.println("Source: " + nbAppletsSource);
        System.out.println("Destination: " + nbAppletsDestination);

        if(!startSynchronizationSession(source, destination))
            return false;

        for(int idAppletSource = 1; idAppletSource <= nbAppletsSource; idAppletSource++) {

            byte[] appletAID = getAIDById(source, idAppletSource, nbAppletsSource);
            if(appletAID.length == 0)
                return false;

            int idAppletDestination = 0;
            try {
                idAppletDestination = checkInstallByAID(destination, appletAID);
            } catch(AppletNotFoundException e) {
                    System.out.println("Applet not found... Trying to install it from library");

                    InputStream input = library.getAppletCAPByAID(appletAID);
                    long size = library.getAppletCAPSizeByAID(appletAID);

                    if(!destination.uploadAppletCAPFile(input, size)) {
                        System.out.println("Could not upload the new applet");
                        return false;
                    }

                    if(!destination.createAppletInstance(appletAID)) {
                        System.out.println("Could not instantiate the new applet");
                        return false;
                    }

                    nbAppletsDestination++;

                    try {
                        if(!destination.selectAppletByAID(sync_AID)) {
                            System.err.println("Could not select the sync manager applet within the destination device");
                            return false;
                        }
                    }
                    catch(AppletNotFoundException x) {
                        throw new UnsupportedDeviceException("The destination device is not sync compliant");
                    }

                    idAppletDestination = checkInstallByAID(destination, appletAID);

            }
            if(idAppletDestination == 0)
                return false;

            if(!synchronizeAppletToApplet(source, idAppletSource, nbAppletsSource, destination, idAppletDestination, nbAppletsDestination))
                return false;

        }

        return true;

    }


    /**
     * Shutdown the dock.
     */
    public void shutdown() {

        Iterator<Integer> it = list.keySet().iterator();
        while(it.hasNext())
            list.get(it.next()).disconnect();

    }

}
