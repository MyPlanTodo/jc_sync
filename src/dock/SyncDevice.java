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

import com.sun.javacard.apduio.Apdu;
import java.io.InputStream;

/**
 * This interface defines the methods that an sync device implementing class should provide.
 */
public interface SyncDevice {

    /**
     * Connect this sync device for use.
     */
    public boolean connect();

    /**
     * Disconnect this sync device after use.
     */
    public boolean disconnect();

    /**
     * Send an apdu to this sync device.
     * @param apdu The apdu to send.
     * @return Return false if successful, false else.
     */
    public boolean exchangeApdu(Apdu apdu);

    /**
     * Select an applet on this sync device given the AID.
     * @param appletAID The AID of the applet to select.
     * @return Return true if successful, false else.
     */
    public boolean selectAppletByAID(byte[] appletAID) throws AppletNotFoundException;

    /**
     * Upload a CAP file to this sync device.
     * @param CAPFile An InputStream from which the CAP file is read.
     * @param CAPFileSize The size in byte of the CAP file.
     * @return Return true if successful, false else.
     */
    public boolean uploadAppletCAPFile(InputStream CAPFile, long CAPFileSize);

    /**
     * Create an instance of an applet given its AID.
     * @param appletAID The AID of the applet for which an instance should be created.
     * @return Return true if successful, false else.
     */
    public boolean createAppletInstance(byte[] appletAID);

    /**
     * Delete an applet instance given its AID.
     * @param appletAID The AID of the applet which instance should be deleted.
     * @return Return true if successful, false else.
     */
    public boolean deleteAppletByAID(byte[] appletAID);

    /**
     * Delete a package given its AID.
     * @param packageAID The AID of the package whick should be deleted.
     * @return Return true if successful, false else.
     */
    public boolean deletePackageByAID(byte[] packageAID);

    /**
     * Delete a package and its instances given the package's AID.
     * @param packageAID The AID of the package which instances and date should be deleted.
     * @return Return true if successful, false else.
     */
    public boolean deletePackageAndAppletsByAID(byte[] packageAID);

}
