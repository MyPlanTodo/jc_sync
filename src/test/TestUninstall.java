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


public class TestUninstall {
    private static final byte[] testApplet1AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x03, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet1PackageAID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x03, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x00};

    private static final byte[] testApplet2AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x04, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet2PackageAID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x04, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x00};
    private static final byte[] testApplet3AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x05, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet3PackageAID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x05, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x00};
    private static final byte[] syncPackageAID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x00};
    private static final byte[] syncLibPackageAID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x01, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x00};


    private static boolean testUninstallApplets(Dock dock, int deviceIndex, int nbApplets) {

        for(int i = 0; i < nbApplets; i++) {
            System.out.println("Removing applet " + (i + 1));
            try{
                if(!dock.deleteAppletById(deviceIndex, 1, nbApplets-i))
                    return false;
            }
            catch(SyncException e) {
                System.err.println(e.getMessage());
                return false;
            }
            catch(IndexOutOfBoundsException e) {
                System.err.println(e.getMessage());
                return false;
            }
            catch(UnsupportedDeviceException e) {
                System.err.println(e.getMessage());
                return false;
            }

            int remainingApplets = -1;
            try {
                remainingApplets = dock.getNbManagedApplets(deviceIndex);
            }
            catch(UnsupportedDeviceException e) {
                System.err.println("device "+deviceIndex+": "+e.getMessage());
                return false;
            }
            if(remainingApplets == -1)
                return false;

            System.out.println("There is " + remainingApplets + " applet(s) remaining on the device " + deviceIndex);

        }

        System.out.println("removing SyncManager");
        if(!dock.deleteAppletByAID(deviceIndex, Dock.sync_AID))
            return false;

        return true;

    }


    private static boolean testDeletePackages(Dock dock, int deviceIndex, int nbApplets) {

        switch(nbApplets) {
            case 3:
                if(!dock.deletePackageByAID(deviceIndex, testApplet3PackageAID))
                    return false;
                System.out.println("Applet 3 package deleted");

            case 2:
                if(!dock.deletePackageByAID(deviceIndex, testApplet2PackageAID))
                    return false;
                System.out.println("Applet 2 package deleted");

            case 1:
                if(!dock.deletePackageByAID(deviceIndex, testApplet1PackageAID))
                    return false;
                System.out.println("Applet 1 package deleted");

            default:
        }

        if(!dock.deletePackageByAID(deviceIndex, syncPackageAID))
            return false;
        System.out.println("syncManager package deleted");

        if(!dock.deletePackageByAID(deviceIndex, syncLibPackageAID))
            return false;
        System.out.println("sync library package deleted");

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

            System.out.println("There is " + nbApplets + " applet(s) in device " + deviceIndex[i]);

            if(!testUninstallApplets(dock, deviceIndex[i], nbApplets))
                return false;

            System.out.println("Applets on device " + deviceIndex[i] + " deleted.");

            if(!testDeletePackages(dock, deviceIndex[i], nbApplets))
                return false;

            System.out.println("Package on device " + deviceIndex[i] + " deleted");

        }

        for(int i = 0; i < eepromsPath.length; i++) {

            dock.removeSyncDevice(deviceIndex[i]).disconnect();
            System.out.println("sync device " + deviceIndex[i] + " removed");

        }

        return true;

    }

}
