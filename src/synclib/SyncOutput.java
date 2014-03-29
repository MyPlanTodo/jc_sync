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
import javacard.framework.UserException;
import javacard.framework.ISOException;

/**
 * This shareable interface describes which methods an sync applet will use to write serialize data.
 */
public interface SyncOutput extends Shareable {

    /**
     * Start the write process.
     */
    public void start(AID appletAID);

    /**
     * Resume the write process.
     */
    public void resume();

    /**
     * Replay the last write process.
     */
    public void replay() throws UserException;

    /**
     * Reset the resume process as to avoid unneeded ghost writings.
     */
    public void resetResume();

    /**
     * Write a byte.
     * @param b The byte to write.
     */
    public void write(byte b) throws ISOException, UserException;

    /**
     * Write a short.
     * @param s The short to write.
     */
    public void write(short s) throws ISOException, UserException;

    /**
     * Terminate the write process.
     */
    public void done() throws ISOException;

}
