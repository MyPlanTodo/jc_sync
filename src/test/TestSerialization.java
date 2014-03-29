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
import dock.IncompleteMergeException;

import java.util.ArrayList;

public class TestSerialization {

    private static final byte[] testApplet1AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x03, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet2AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x04, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};
    private static final byte[] testApplet3AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x05, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};

/*
    private static ArrayList<byte[]> testSerialize(Dock dock, int deviceIndex, int appletId, int nbApplets) {

        ArrayList<byte[]> data = null;
        try {
            data = dock.serializeAppletById(deviceIndex, appletId, nbApplets);
        }
        catch(SyncException e) {
            System.err.println(e.getMessage());
            return null;
        }
        catch(IndexOutOfBoundsException e) {
            System.err.println(e.getMessage());
            return null;
        }
        catch(UnsupportedDeviceException e) {
            System.err.println(e.getMessage());
            return null;
        }
        catch(IncompleteMergeException e) {
            System.err.println(e.getMessage());
            return null;
        }

        if(data == null) {
            System.err.println("The serialization failled");
            return null;
        }

        return data;

    }

    private static boolean testMerge(Dock dock, int deviceIndex, int appletId, int nbApplets, ArrayList<byte[]> data) {

        boolean result = false;

        try {
            result = dock.mergeAppletById(deviceIndex, appletId, nbApplets, data);
        }
        catch(SyncException e) {
            System.err.println(e.getMessage());
            return false;
        }
        catch(IndexOutOfBoundsException e) {
            System.err.println(e.getMessage());
            return false;
        }
        catch(IncompatibleArrayLengthException e) {
            System.err.println(e.getMessage());
            return false;
        }
        catch(UnsupportedDeviceException e) {
            System.err.println(e.getMessage());
            return false;
        }

        return result;

    }


    private static boolean testSerialization1(Dock dock, int deviceIndex, int nbApplets, SyncDevice device) {

        ArrayList<byte[]> data = null;
        ArrayList<byte[]> newData = null;
        short newSize = 0;

        switch(nbApplets) {

            case 3:
                System.out.println("Serialization of applet 3");

                data = testSerialize(dock, deviceIndex, 3, nbApplets);
                if(data == null)
                    return false;
*/
/*
                System.out.println("Applet 3 data:");
                if(data.get(0).length == 0)
                    System.out.println("No data");
                else
                    System.out.println(TestUtil.dataToString(data));
*/
/*
                if(!testMerge(dock, deviceIndex, 3, nbApplets, data))
                    return false;

                System.out.println("Applet 3 data merged");

                newData = testSerialize(dock, deviceIndex, 3, nbApplets);
                if(newData == null)
                    return false;

                if(TestUtil.dataToString(data).equals(TestUtil.dataToString(newData))) {
                    System.out.println("Data are the same after merge");
                } else {
                    System.err.println("Data are not the same after merge:");
                    System.err.println(TestUtil.dataToString(data));
                    System.err.println(TestUtil.dataToString(newData));
                    return false;
                }

            case 2:
                System.out.println("Serialization of applet 2");

                data = testSerialize(dock, deviceIndex, 2, nbApplets);
                if(data == null)
                    return false;
*/
/*
                System.out.println("Applet 3 data:");
                if(data.get(0).length == 0)
                    System.out.println("No data");
                else
                    System.out.println(TestUtil.dataToString(data));
*/
/*
                TestUtil.setMonopArraySize(device, testApplet2AID, 0, 64);
                newSize = TestUtil.getMonopArraySize(device, testApplet2AID, 0);
                if(newSize == (short)64) {
                    System.out.println("The first array of the second applet is now 64 bytes long");
                } else {
                    System.err.println("The resizing failled. Should be 64, is "+newSize);
                    return false;
                }
                TestUtil.setMonopArraySize(device, testApplet2AID, 2, 16);
                newSize = TestUtil.getMonopArraySize(device, testApplet2AID, 2);
                if(newSize == (short)16) {
                    System.out.println("The third array of the second applet is now 32 bytes long");
                } else {
                    System.err.println("The resizing failled. Should be 16, is "+newSize);
                    return false;
                }

                TestUtil.changeMonopArrayValues(device, testApplet2AID);

                if(!testMerge(dock, deviceIndex, 2, nbApplets, data))
                    return false;

                System.out.println("Applet 2 data merged");

                newData = testSerialize(dock, deviceIndex, 2, nbApplets);
                if(newData == null)
                    return false;

                if(TestUtil.dataToString(data).equals(TestUtil.dataToString(newData))) {
                    System.out.println("Data are the same after merge");
                } else {
                    System.err.println("Data are not the same after merge:");
                    System.err.println(TestUtil.dataToString(newData));
                    return false;
                }

            case 1:
                System.out.println("Serialization of applet 1");

                data = testSerialize(dock, deviceIndex, 1, nbApplets);
                if(data == null)
                    return false;
*/
/*
                System.out.println("Applet 1 data:");
                if(data.get(0).length == 0)
                    System.out.println("No data");
                else
                    System.out.println(TestUtil.dataToString(data));
*/
/*
                TestUtil.changeMonopArrayValues(device, testApplet1AID);

                if(!testMerge(dock, deviceIndex, 1, nbApplets, data))
                    return false;

                System.out.println("Applet 1 data merged");

                newData = testSerialize(dock, deviceIndex, 1, nbApplets);
                if(newData == null)
                    return false;

                if(TestUtil.dataToString(data).equals(TestUtil.dataToString(newData))) {
                    System.out.println("Data are the same after merge");
                } else {
                    System.err.println("Data are not the same after merge:");
                    System.err.println(TestUtil.dataToString(newData));
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
        SyncDevice[] devices = new syncDevice[eepromsPath.length];

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

            if(!testSerialization1(dock, deviceIndex[i], nbApplets, devices[i]))
                return false;

        }

        for(int i = 0; i < eepromsPath.length; i++) {

            dock.removeSyncDevice(deviceIndex[i]).disconnect();
            System.out.println("sync device " + deviceIndex[i] + " removed");

        }

        return true;

    }
*/
}
