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


public class TestSynchronization {

    private static final byte[] testApplet1AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x03, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet2AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x04, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet3AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x05, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};


    public static boolean start(AppletLibrary library, String[] eepromsPath) {

        String newEeprom = null;

        try {
            newEeprom = TestUtil.copyEeprom(eepromsPath[0]);
        }
        catch(Exception e) {
            System.err.println("Copy failed");
            return false;
        }
        SyncDevice internalDevice = new CrefSyncDevice(newEeprom);
        internalDevice.connect(); 

        if(!TestUtil.installUserkey(internalDevice))
            return false;

        Dock dock = new Dock(library, internalDevice);
        System.out.println("Dock initiliazed with: "+eepromsPath[0]);

        if(!TestUtil.changeTestAppletArrayValues(internalDevice, testApplet1AID, true)) {
            System.err.println("Could not tweak applet data");
            return false;
        }

        if(!TestUtil.changeTestAppletArrayValues(internalDevice, testApplet2AID, false)) {
            System.err.println("Could not tweak applet data");
            return false;
        }

        if(!TestUtil.changeTestAppletArrayValues(internalDevice, testApplet3AID, true)) {
            System.err.println("Could not tweak applet data");
            return false;
        }


        int[] deviceIndex = new int[eepromsPath.length];
        SyncDevice[] devices = new SyncDevice[eepromsPath.length];

        for(int i = 0; i < eepromsPath.length; i++) {

            try {
                newEeprom = TestUtil.copyEeprom(eepromsPath[i]);
            }
            catch(Exception e) {
                System.err.println("Copy failed");
                return false;
            }

            devices[i] = new CrefSyncDevice(newEeprom);

            devices[i].connect();

            if(!TestUtil.installUserkey(devices[i]))
                return false;

            deviceIndex[i] = dock.addSyncDevice(devices[i]);
            if(deviceIndex[i] == -1)
                return false;

            System.out.println("sync device " + deviceIndex[i] + " added (" + eepromsPath[i] + ")");
        }

        for(int i = 0; i < eepromsPath.length; i++) {

            try {
                if(dock.synchronizeOneToOne(internalDevice, devices[i]))
                    System.out.println("Synchronization done");
                else
                    System.err.println("Synchronization failed");
            }
            catch(SyncException e) {
                System.err.println(e.getMessage());
                return false;
            }
            catch(IndexOutOfBoundsException e) {
                System.err.println(e.getMessage());
            return false;
            }
            catch(AppletNotRegisteredException e) {
                System.err.println(e.getMessage());
                return false;
            }
            catch(AppletNotFoundException e) {
                System.err.println(e.getMessage());
                return false;
            }
            catch(UnsupportedDeviceException e) {
                System.err.println(e.getMessage());
                return false;
            }

            if(!TestUtil.have_same_data(internalDevice, devices[i], testApplet1AID, true)) {
                System.out.println("internal device and device "+i+" have not the same data for applet 1");
                return false;
            } else {
                System.out.println("internal device and device "+i+" have the same data for applet 1");
            }

            if(!TestUtil.have_same_data(internalDevice, devices[i], testApplet2AID, false)) {
                System.out.println("internal device and device "+i+" have not the same data for applet 1");
                return false;
            } else {
                System.out.println("internal device and device "+i+" have the same data for applet 2");
            }

            if(!TestUtil.have_same_data(internalDevice, devices[i], testApplet3AID, true)) {
                System.out.println("internal device and device "+i+" have not the same data for applet 1");
                return false;
            } else {
                System.out.println("internal device and device "+i+" have the same data for applet 3");
            }

        }

        for(int i = 0; i < eepromsPath.length; i++) {

            dock.removeSyncDevice(deviceIndex[i]).disconnect();
            System.out.println("sync device " + deviceIndex[i] + " removed");

        }

        return true;

    }

}
