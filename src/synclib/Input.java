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

import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;
import javacard.framework.UserException;
import javacard.framework.Util;

/**
 * This class represents an input source from which an applet read the merge data.  
 */
public final class Input implements ResetIdSource {

    /**
     * If used as a reason for an UserException, indicates that a new instance is available.
     */
    public final static short NEW_INSTANCE = (short)-1;

    /**
     * If used as a reason for an UserException, indicates that the current instance should be skipped.
     */
    public final static short SKIP_INSTANCE = (short)-2;

    /**
     * The shareable interface from which the data will be read.
     */ 
    private SyncInput in = null;

    /**
     * Use to pass around a newly created Synchronizable instance (probably using {@link SyncApplet#getNewInstance(Input, short)}.)
     */
    public Synchronizable newlyCreatedInstance = null;

    /**
     * was an array resized?
     */
    private boolean wasArrayResized = false;

    /**
     * The current {@link SyncApplet} instance being merged.
     * @see SyncApplet#getNewInstance(Input, short)
     * @see SyncApplet#getClassId(Synchronizable)
     */
    SyncApplet currentApplet = null;

    /**
     * The current value used to reset id of Synchronizable instances.
     */
    private short resetId = (short)0;

    /**
     * Is the currently being merged array a duplicate reference ?
     */
    private static boolean isDuplicateReference = false;

    /**
     * Create an instance linked with an {@link SyncApplet} instance.
     * @param applet The applet linked to this input.
     */
    Input(SyncApplet applet) {
        this.currentApplet = applet;
    }

    /**
     * Start the read process.
     * @param in The shareable interface to read from.
     * @throws ISOException An ISOException exception can be thrown by {@link SyncInput#start(javacard.framework.AID)}.
     * @see sync.SyncInputCryptoAPDU#start(javacard.framework.AID)
     */
    void start(SyncInput in)
        throws ISOException {

        this.in = in;
        this.in.start(JCSystem.getAID());
        --resetId;

        if(wasArrayResized)
            wasArrayResized = false;
        if(isDuplicateReference)
            isDuplicateReference = false;

    }


    /**
     * Resume the read process.
     * @throws ISOException An ISOException exception can be thrown by {@link SyncInput#resume()}.
     * @see sync.SyncInputCryptoAPDU#resume()
     */
    void resume()
        throws ISOException {

        in.resume();

    }


    /**
     * Get the current value used to reset id of a Synchronizable instance.
     * @return The reset value.
     */
    public short getResetId() {

        return resetId;

    }


    /**
     * Reset the resume mechanism of the input. Usefull if when resuming the previous data are not available for ghost reading.
     */
    public void resetResume() {
        in.resetResume();
    }


    /**
     * Read a boolean.
     * @param b The previous value of the read boolean.
     * @return The read boolean.
     * @throws UserException An UserException can be thrown by a read method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#read(byte)
     */
    public boolean read(boolean b)
        throws UserException {

        return (in.read((byte)(b ? 1 : 0)) == 1) ? true : false;

    }


    /**
     * Read a byte.
     * @param b The previous value of the read byte.
     * @return The read byte.
     * @throws UserException An UserException can be thrown by a read method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#read(byte)
     */
    public byte read(byte b)
        throws UserException {

        return in.read(b);

    }


    /**
     * Read a short.
     * @param s The previous value of the read short.
     * @return The read short.
     * @throws UserException An UserException can be thrown by a read method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#read(short)
     */
    public short read(short s)
        throws UserException {

        return in.read(s);

    }


    /**
     * Check the size and type (transient or not) of a boolean array.
     * @param array The boolean array to check.
     * @return The checked array which might have resized.
     * @throws ISOException An ISOException is thrown if the read array size differ from the given and the object deletion method is not supported.
     * @throws UserException An UserException can be thrown by a read method of an SyncInput instance.
     * @see ISO7816#SW_FUNC_NOT_SUPPORTED
     * @see sync.SyncInputCryptoAPDU#read(short)
     * @see SyncApplet#getDuplicatedBooleanArray(short, short)
     */
    public boolean[] checkArray(boolean[] array)
        throws ISOException, UserException {

        if(JCSystem.isTransient(array) != JCSystem.NOT_A_TRANSIENT_OBJECT)
            return array;

        if(in.readN((short)5)) {
            if(in.getReadNValue((short)0) == Output.IS_DUPLICATE_REFERENCE) {
                isDuplicateReference = true;
                return currentApplet.getDuplicatedBooleanArray((short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2)), (short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4)));
            } else {
                short length = (short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2));
                boolean[] oldArray = null;

                if(array == null) {
                    array = new boolean[length];
                } else if(length != array.length) {
                    if(!JCSystem.isObjectDeletionSupported())
                        ISOException.throwIt(ISO7816.SW_FUNC_NOT_SUPPORTED);

                    oldArray = array;
                    array = new boolean[length];

                    if(length < oldArray.length) {
                        for(short i = (short)0; i < length; ++i)
                            array[i] = oldArray[i];
                    } else {
                        for(short i = (short)0; i < oldArray.length; ++i)
                            array[i] = oldArray[i];
                        for(short i = (short)oldArray.length; i < (short)(length - oldArray.length); ++i)
                            array[i] = false;
                    }
                }

                if(oldArray != null) {
                    wasArrayResized = true;
                    JCSystem.requestObjectDeletion();
                }
            }
        }

        return array;

    }


    /**
     * Check the size and type (transient or not) of a byte array.
     * @param array The byte array to check.
     * @return The checked array which might have resized.
     * @throws ISOException An ISOException is thrown if the read array size differ from the given and the object deletion method is not supported.
     * @throws UserException An UserException can be thrown by a read method of an SyncInput instance.
     * @see ISO7816#SW_FUNC_NOT_SUPPORTED
     * @see sync.SyncInputCryptoAPDU#read(short)
     * @see SyncApplet#getDuplicatedByteArray(short, short)
     */
    public byte[] checkArray(byte[] array)
        throws ISOException, UserException {

        if(JCSystem.isTransient(array) != JCSystem.NOT_A_TRANSIENT_OBJECT)
            return array;

        if(in.readN((short)5)) {
            if(in.getReadNValue((short)0) == Output.IS_DUPLICATE_REFERENCE) {
                isDuplicateReference = true;
                return currentApplet.getDuplicatedByteArray((short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2)), (short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4)));
            } else {
                short length = (short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2));
                byte[] oldArray = null;

                if(array == null) {
                    array = new byte[length];
                } else if(length != array.length) {
                    if(!JCSystem.isObjectDeletionSupported())
                        ISOException.throwIt(ISO7816.SW_FUNC_NOT_SUPPORTED);

                    oldArray = array;
                    array = new byte[length];

                    if(length < oldArray.length) {
                        for(short i = (short)0; i < length; ++i)
                            array[i] = oldArray[i];
                    } else {
                        for(short i = (short)0; i < oldArray.length; ++i)
                            array[i] = oldArray[i];
                        for(short i = (short)oldArray.length; i < (short)(length - oldArray.length); ++i)
                            array[i] = (byte)0;
                    }
                }

                if(oldArray != null) {
                    wasArrayResized = true;
                    JCSystem.requestObjectDeletion();
                }
            }
        }

        return array;

    }


    /**
     * Check the size and type (transient or not) of a short array.
     * @param array The short array to check.
     * @return The checked array which might have resized.
     * @throws ISOException An ISOException is thrown if the read array size differ from the given and the object deletion method is not supported.
     * @throws UserException An UserException can be thrown by a read method of an SyncInput instance.
     * @see ISO7816#SW_FUNC_NOT_SUPPORTED
     * @see sync.SyncInputCryptoAPDU#read(short)
     * @see SyncApplet#getDuplicatedShortArray(short, short)
     */
    public short[] checkArray(short[] array)
        throws ISOException, UserException {

        if(JCSystem.isTransient(array) != JCSystem.NOT_A_TRANSIENT_OBJECT)
            return array;

        if(in.readN((short)5)) {
            if(in.getReadNValue((short)0) == Output.IS_DUPLICATE_REFERENCE) {
                isDuplicateReference = true;
                return currentApplet.getDuplicatedShortArray((short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2)), (short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4)));
            } else {
                short length = (short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2));
                short[] oldArray = null;

                if(array == null) {
                    array = new short[length];
                } else if(length != array.length) {
                    if(!JCSystem.isObjectDeletionSupported())
                        ISOException.throwIt(ISO7816.SW_FUNC_NOT_SUPPORTED);

                    oldArray = array;
                    array = new short[length];

                    if(length < oldArray.length) {
                        for(short i = (short)0; i < length; ++i)
                            array[i] = oldArray[i];
                    } else {
                        for(short i = (short)0; i < oldArray.length; ++i)
                            array[i] = oldArray[i];
                        for(short i = (short)oldArray.length; i < (short)(length - oldArray.length); ++i)
                            array[i] = (short)0;
                    }
               }

                if(oldArray != null) {
                    wasArrayResized = true;
                    JCSystem.requestObjectDeletion();
                }
            }
        }

        return array;

    }


    /**
     * Read a boolean array.
     * @param array The previous boolean array.
     * @throws UserException An UserException can be thrown by a replay or read method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#replay()
     * @see sync.SyncInputCryptoAPDU#read(byte)
     */
    public void read(boolean[] array)
        throws UserException {

        if(JCSystem.isTransient(array) != JCSystem.NOT_A_TRANSIENT_OBJECT)
            return;

        if(isDuplicateReference) {
            isDuplicateReference = false;
            return;
        }

        if(wasArrayResized) {
            wasArrayResized = false;
            in.replay();
        }

        for(short i = (short)0; i < array.length; i++)
            array[i] = (in.read((byte)(array[i] ? 1 : 0)) == 1) ? true : false;

    }


    /**
     * Read a byte array and eventually return a resized array if the size differ.
     * @param array The previous byte array.
     * @throws UserException An UserException can be thrown by a replay or read method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#replay()
     * @see sync.SyncInputCryptoAPDU#read(byte)
     */
    public void read(byte[] array)
        throws UserException {

        if(JCSystem.isTransient(array) != JCSystem.NOT_A_TRANSIENT_OBJECT)
            return;

        if(isDuplicateReference) {
            isDuplicateReference = false;
            return;
        }

        if(wasArrayResized) {
            wasArrayResized = false;
            in.replay();
        }

        for(short i = (short)0; i < array.length; i++)
            array[i] = in.read(array[i]);

    }


    /**
     * Read a short array and eventually return a resized array if the size differ.
     * @param array The previous short array.
     * @throws UserException An UserException can be thrown by a replay or read method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#replay()
     * @see sync.SyncInputCryptoAPDU#read(short)
     */
    public void read(short[] array)
        throws UserException {

        if(JCSystem.isTransient(array) != JCSystem.NOT_A_TRANSIENT_OBJECT)
            return;

        if(isDuplicateReference) {
            isDuplicateReference = false;
            return;
        }

        if(wasArrayResized) {
            wasArrayResized = false;
            in.replay();
        }

        for(short i = (short)0; i < array.length; i++)
            array[i] = in.read(array[i]);

    }


    /**
     * Read n bytes to access them as an array.
     * @param n The number of bytes to read; should be less than the result from getMaxAPDUBufferLength method.
     * @return Return True if the reading was just completed, false if the reading was a ghost one.
     * @throws ISOException An ISOException exception can be thrown by a readN method of an SyncInput instance.
     * @throws UserException An UserException exception can be thrown by a readN method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#readN(short)
     */
    public boolean readN(short n)
        throws ISOException, UserException {
        
        return in.readN(n);

    }


    /**
     * Get a value read by the readN method.
     * @param i The index of the value to read.
     * @return The i-th read value.
     * @throws ISOException An ISOException exception can be thrown by a getReadNValue method of an SyncInput instance.
     * @see sync.SyncInputCryptoAPDU#getReadNValue(short)
     */
    public byte getReadNValue(short i) {

        return in.getReadNValue(i);

    }


    /**
     * Terminate the read process.
     */
    void done() {

        ++resetId;
        in.done();
        in = null;

    }

}
