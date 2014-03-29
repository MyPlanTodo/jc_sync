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

import dock.Dock;
import dock.SyncException;
import dock.IncompatibleArrayLengthException;
import dock.AppletNotFoundException;
import dock.AppletNotRegisteredException;
import dock.CrefSyncDevice;
import dock.SyncDevice;
import dock.AppletLibrary;
import dock.UnsupportedDeviceException;

public class TestCheck {
    private static final byte[] testApplet1AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x03, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet2AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x04, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet3AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x05, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};

    private static boolean testErrorHandling(Dock dock, int deviceIndex, int nbApplets) {
        boolean gotException = false;
        byte[] AID;

        System.out.println("Asking 0th applet from " + nbApplets);

        try {
            AID = dock.getAIDById(deviceIndex, 0, nbApplets);
        }
        catch(SyncException e) {
            System.err.println("Expected IndexOutOfBoundsException, got:");
            System.err.println(e.toString());
            return false;
        }
        catch(IndexOutOfBoundsException e) {
            System.out.println("->failling like it should");
            gotException = true;
        }
        catch(UnsupportedDeviceException e) {
            System.err.println(e.getMessage());
            return false;
        }

        if(!gotException) {
            System.err.println("Expected IndexOutOfBoundsException.");
            return false;
        }

        gotException = false;

        System.out.println("Asking " + (nbApplets + 1) + "th applet from " + nbApplets);

        try {
            AID = dock.getAIDById(deviceIndex, nbApplets+1, nbApplets);
        }
        catch(SyncException e) {
            System.err.println("Expected IndexOutOfBoundsException, got:");
            System.err.println(e.toString());
            return false;
        }
        catch(IndexOutOfBoundsException e) {
            System.out.println("->failling like it should");
            gotException = true;
        }
        catch(UnsupportedDeviceException e) {
            System.err.println(e.getMessage());
            return false;
        }

        if(!gotException) {
            System.err.println("Expected IndexOutOfBoundsException.");
            return false;
        }
        
        gotException = false;

        System.out.println("Asking 1st applet from 0");

        try {
            AID = dock.getAIDById(deviceIndex, 1, 0);
        }
        catch(SyncException e) {
            if(nbApplets == 0) {
                System.err.println("Excepted IndexOutOfBoundException, got:");
                System.err.println(e.getMessage());
                return false;
            } else {
                System.out.println("->failling like it should");
                gotException = true;
            }
        }
        catch(IndexOutOfBoundsException e) {
            if(nbApplets == 0) {
                System.out.println("->failling like it should");
                gotException = true;
            } else {
                System.err.println("Expected SyncException, got:");
                System.err.println(e.getMessage());
                return false;
            }
        }
        catch(UnsupportedDeviceException e) {
            System.err.println(e.getMessage());
            return false;
        }

        if(!gotException) {
            System.err.println("Expected SyncException.");
            return false;
        }

        gotException = false;

        System.out.println("Asking 1st applet from " + (nbApplets + 1));

        try {
            AID = dock.getAIDById(deviceIndex, 1, nbApplets+1);
        }
        catch(SyncException e) {
            System.out.println("->failling like it should");
            gotException = true;
        }
        catch(IndexOutOfBoundsException e) {
            System.err.println("Expected SyncException, got:");
            System.err.println(e.toString());
            return false;
        }
        catch(UnsupportedDeviceException e) {
            System.err.println(e.getMessage());
            return false;
        }

        if(!gotException) {
            System.err.println("Expected SyncException.");
            return false;
        }

        return true;

    }


    private static boolean testCheckInstall(Dock dock, int deviceIndex, int nbApplets) {

        switch(nbApplets) {
            case 3:
                try {
                    if(!dock.checkInstallByAID(deviceIndex, testApplet3AID))
                        System.err.println("Applet 3 is not registered within sync device " + deviceIndex);
                    else
                        System.out.println("Applet 3 is correctly registered within sync device " + deviceIndex);
                }
                catch(UnsupportedDeviceException e) {
                    System.err.println(e.getMessage());
                    return false;
                }

            case 2:
                try {
                    if(!dock.checkInstallByAID(deviceIndex, testApplet2AID))
                        System.err.println("Applet 2 is not registered within sync device " + deviceIndex);
                    else
                        System.out.println("Applet 2 is correctly registered within sync device " + deviceIndex);
                }
                catch(UnsupportedDeviceException e) {
                    System.err.println(e.getMessage());
                    return false;
                }

            case 1:
                try {
                    if(!dock.checkInstallByAID(deviceIndex, testApplet1AID))
                        System.err.println("Applet 1 is not registered within sync device " + deviceIndex);
                    else
                        System.out.println("Applet 1 is correctly registered within sync device " + deviceIndex);
                }
                catch(UnsupportedDeviceException e) {
                    System.err.println(e.getMessage());
                    return false;
                }
            default:
        }

        return true;

    }

    public static boolean start(AppletLibrary library, String[] eepromsPath) {

        String newEeprom = null;
        Dock dock = new Dock(library);
        int[] deviceIndex = new int[eepromsPath.length];

        for(int i = 0; i < eepromsPath.length; i++) {

            try {
                newEeprom = TestUtil.copyEeprom(eepromsPath[i]);
            }
            catch(Exception e) {
                System.err.println("Copy failed");
                return false;
            }

            SyncDevice device = new CrefSyncDevice(newEeprom);

            device.connect();

            if(!TestUtil.installUserkey(device))
                return false;

            deviceIndex[i] = dock.addSyncDevice(device);
            if(deviceIndex[i] == -1)
                return false;

            System.out.println("sync device " + deviceIndex[i] + " added (" + eepromsPath[i] + ")");
        }


        for(int i = 0; i < deviceIndex.length; i++) {

            System.out.println(">>>>>>>>>>>> DEVICE "+deviceIndex[i] + " <<<<<<<<<<<<");

            int nbApplets = -1;
            try {
                nbApplets = dock.getNbManagedApplets(deviceIndex[i]);
            }
            catch(UnsupportedDeviceException e) {
                System.err.println("device "+deviceIndex[i]+": "+e.getMessage());
                return false;
            }
            if(nbApplets == -1)
                return false;

            if(!testCheckInstall(dock, deviceIndex[i], nbApplets))
                return false;

            if(!testErrorHandling(dock, deviceIndex[i], nbApplets))
                return false;

        }

        for(int i = 0; i < eepromsPath.length; i++) {

            dock.removeSyncDevice(deviceIndex[i]).disconnect();
            System.out.println("sync device " + deviceIndex[i] + " removed");

        }

        return true;

    }

}
