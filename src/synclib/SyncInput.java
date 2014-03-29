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

package synclib;

import javacard.framework.AID;
import javacard.framework.Shareable;
import javacard.framework.ISOException;
import javacard.framework.UserException;

/**
 * This shareable interface describes which methods an sync applet will use to read merge data.
 */
public interface SyncInput extends Shareable {

    /**
     * This value is used as a reason for an UserException when the merge process wait for more.
     */
    public static final byte WAITING_FOR_MORE = (byte)0x34;

    /**
     * This value is used as a reason for an UserException when the merge process need the current APDU to be sent again.
     * It is used to force the completion of the deletion request.
     */
    public static final byte REPLAY_THIS_APDU = (byte)0x42;

    /**
     * Start the read process.
     * @param appletAID The AID of the applet being mergerd.
     */
    public void start(AID appletAID) throws ISOException;

    /**
     * Ask for replaying the current read process.
     */
    public void replay() throws UserException;

    /**
     * Resume the read process.
     */
    public void resume() throws ISOException;

    /**
     * Reset the resume process as to avoid unneeded ghost readings.
     */
    public void resetResume();

    /**
     * Read a byte.
     * @param b The previous value of the read byte.
     * @return The read byte.
     */
    public byte read(byte b) throws UserException;

    /**
     * Read a short.
     * @param s The previous value of the read short.
     * @return The read short.
     */
    public short read(short s) throws UserException;

    /**
     * Read n bytes to access them as an array.
     * @param n The number of bytes to read.
     * @return Return True if the reading was just completed, false if the reading was a ghost one.
     */
    public boolean readN(short n) throws ISOException, UserException;

    /**
     * Get a value read by the readN method.
     * @param i The index of the value to read.
     * @return The i-th read value.
     */
    public byte getReadNValue(short i) throws ISOException;

    /**
     * Terminate the read process.
     */
    public void done();

}
