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

public class TestPrint {

    private static boolean testPrint(Dock dock, int deviceIndex, int nbApplets) {

        byte[] AID;

        System.out.println("Listing of the sync device " + deviceIndex + ":");
        try {
            if(!dock.printManagedAppletsAID(deviceIndex))
                return false;
        }
        catch(UnsupportedDeviceException e) {
            System.err.println(e.getMessage());
            return false;
        }

        for(int i = 1; i <= nbApplets; i++) {


            try {
                AID = dock.getAIDById(deviceIndex, i, nbApplets);
            }
            catch(SyncException e) {
                System.err.println(e.toString());
                return false;
            }
            catch(IndexOutOfBoundsException e) {
                System.err.println(e.toString());
                return false;
            }
            catch(UnsupportedDeviceException e) {
                System.err.println(e.getMessage());
                return false;
            }
            

            StringBuffer AIDStringHex = new StringBuffer();

            for(int j = 0; j < AID.length; j++) {
                String hex = Integer.toHexString(0xFF & AID[j]).toUpperCase();
                if(hex.length() == 1)
                    AIDStringHex.append("0");
                AIDStringHex.append(hex);
                if(j < (AID.length - 1))
                    AIDStringHex.append(":");
            }

            System.out.println("Applet id " + i + ": " + AIDStringHex.toString());
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

            System.out.println("There is " + nbApplets + " applet(s) in device " + deviceIndex[i]);

            if(!testPrint(dock, deviceIndex[i], nbApplets))
                return false;
        }

        for(int i = 0; i < eepromsPath.length; i++) {

            dock.removeSyncDevice(deviceIndex[i]).disconnect();
            System.out.println("sync device " + deviceIndex[i] + " removed");

        }

        return true;

    }

}
