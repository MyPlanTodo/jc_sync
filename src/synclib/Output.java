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

import javacard.framework.JCSystem;
import javacard.framework.UserException;
import javacard.framework.ISOException;

/**
 * This class represents an ouput sink to which an applet write the serialize data. 
 */
public final class Output implements ResetIdSource {

    /**
     * Indicate in the output stream if a Synchronizable instance or an array reference is unique.
     */
    final static byte IS_UNIQUE_REFERENCE = (byte)0;

    /**
     * Indicate in the output stream if a Synchronizable instance possesses a duplicate instance.
     */
    final static byte IS_DUPLICATED_INSTANCE = (byte)1;

    /**
     * Indicate in the output stream if a Synchronizable instance or an array reference is a duplicate instance or reference.
     */
    final static byte IS_DUPLICATE_REFERENCE = (byte)2;

    /**
     * Indicate in the output stream if a Synchronizable or an array reference field is null.
     */
    final static byte IS_NULL_REFERENCE = (byte)3;

    /**
     * Indicate in the output stream if an instance should be skipped or not.
     */
    final static byte SKIP_INSTANCE = (byte)4;

    /**
     * The shareable interface to which the data will be written. 
     */
    private SyncOutput out = null;

    /**
     * The current {@link SyncApplet} instance being serialized.
     */
    SyncApplet currentApplet = null;

    /**
     * The current value used to reset id of Synchronizable instances.
     */
    private short resetId = (short)0;

    /**
     * The current id of Synchronizable instance being serialized.
     */
    private short crtId = (short)0;

    /**
     * Is the currently being serialized array was already being serialized before ?
     */
    private static boolean wasBeingSerialized = false;

    /**
     * Is the currently being serialized array a duplicate reference ?
     */
    private static boolean isDuplicateReference = false;


    /**
     * Create an instance linked with an {@link SyncApplet} instance.
     * @param applet The applet linked to this output.
     */
    Output(SyncApplet applet) {
        this.currentApplet = applet;
    }


    /**
     * Start the write process.
     * @param out The shareable interface to write to.
     */ 
    void start(SyncOutput out) {

        this.out = out;
        this.out.start(JCSystem.getAID());
        crtId = (short)0;
        --resetId;
        if(wasBeingSerialized)
            wasBeingSerialized = false;
        if(isDuplicateReference)
            isDuplicateReference = false;

    }


    /**
     * Resume the write process.
     */
    void resume() {

        out.resume();

    }


    /**
     * Replay the last write process.
     */
    void replay()
        throws UserException {

        out.replay();

    }


    /**
     * Get the current value used to reset id of a Synchronizable instance.
     * @return The reset value.
     */
    public short getResetId() {

        return resetId;

    }


    /**
     * Check whether a Synchronizable instance id indicates that the instance is a duplicate one or not.
     * @param id The id to check.
     * @return Return true if the instance tied to the id is a duplicate one, false else. 
     */
    boolean isDuplicate(short id) {

        return id != crtId;

    }


    /**
     * Get the next id used to number Synchronizable instances.
     * @return The next id.
     */
    public short getNextId() {

        return crtId++;

    }


    /**
     * Reset the current id.
     */
    void resetId() {

        crtId = (short)0;

    }


    /**
     * Tag the current serialized instance as skipped.
     * @throws ISOException An ISOException exception can be thrown be a write method.
     * @throws UserException An UserException exception can be thrown be a write method.
     * @see #write(byte)
     * @see #write(short)
     */
    public void skipInstance()
        throws ISOException, UserException {

        out.write(SKIP_INSTANCE);
        out.write((short)-1);   // padding
        out.write((short)-1);   // padding

    }


    /**
     * Start the serialization of a Synchronizable instance by writing its kind, type and id.
     * @param instance The instance which serialization is starting.
     * @throws ISOException An ISOException exception can be thrown be a write method.
     * @throws UserException An UserException exception can be thrown be a write method.
     * @see #write(byte)
     * @see #write(short)
     */
    void startInstanceSerialization(Synchronizable instance)
        throws ISOException, UserException {

        if(instance == null) {
            out.write(IS_NULL_REFERENCE);
            out.write((short)-1);   // padding
            out.write((short)-1);   // padding
        } else {
            if(instance.getId() < crtId)
                out.write(IS_DUPLICATE_REFERENCE);
            else if(instance.isDuplicated())
                out.write(IS_DUPLICATED_INSTANCE);
            else
                out.write(IS_UNIQUE_REFERENCE);

            out.write(currentApplet.getClassId(instance));
            out.write(instance.getId());
        }

    }


    /**
     * End the serialization of a Synchronizable instance by computing the next id.
     * @param instance The instance which serialization is ending. 
     */
    void endInstanceSerialization(Synchronizable instance) {

        if((instance != null) && (instance.getId() == crtId))
            ++crtId;

    }

    /**
     * Reset the resume mechanism of the output. Usefull if when resuming the previous data are not available for ghost writing.
     */
    public void resetResume() {
        out.resetResume();
    }


    /**
     * Write a boolean.
     * @param b The boolean to write.
     * @throws ISOException An ISOException exception can be throw by a write method of an SyncOuput instance.
     * @throws UserException An UserException exception can be throw by a write method of an SyncOuput instance.
     * @see sync.SyncOutputCryptoAPDU#write(byte)
     */
    public void write(boolean b)
        throws ISOException, UserException {

        out.write((byte)(b ? 1 : 0));

    }


    /**
     * Write a byte.
     * @param b The byte to write.
     * @throws ISOException An ISOException exception can be throw by a write method of an SyncOuput instance.
     * @throws UserException An UserException exception can be throw by a write method of an SyncOuput instance.
     * @see sync.SyncOutputCryptoAPDU#write(byte)
     */
    public void write(byte b)
        throws ISOException, UserException {

        out.write(b);

    }


    /**
     * Write a short.
     * @param s The short to write.
     * @throws ISOException An ISOException exception can be throw by a write method of an SyncOuput instance.
     * @throws UserException An UserException exception can be throw by a write method of an SyncOuput instance.
     * @see sync.SyncOutputCryptoAPDU#write(short)
     */
    public void write(short s)
        throws ISOException, UserException {

        out.write(s);

    }


    /**
     * Write a boolean array.
     * @param array The boolean array to write.
     * @param fieldId The id of the boolean array field. Used for duplicate checking.
     * @throws ISOException An ISOException exception can be throw by a write method of an SyncOuput instance.
     * @throws UserException An UserException exception can be throw by a write method of an SyncOuput instance.
     * @see sync.SyncOutputCryptoAPDU#write(byte)
     * @see sync.SyncOutputCryptoAPDU#write(short)
     */
    public void write(boolean[] array, short fieldId)
        throws ISOException, UserException {

        if(JCSystem.isTransient(array) == JCSystem.NOT_A_TRANSIENT_OBJECT) {
            if(!wasBeingSerialized) {
                isDuplicateReference = currentApplet.isArrayReferenceDuplicate(array, fieldId);
                wasBeingSerialized = true;
            }

            if(isDuplicateReference) {
                out.write(IS_DUPLICATE_REFERENCE);
                out.write(SyncApplet.duplicatedArrayReferenceInstanceId);
                out.write(SyncApplet.duplicatedArrayReferenceFieldId);
            } else {
                out.write(IS_UNIQUE_REFERENCE);
                out.write((short)array.length);
                out.write((short)-1); // padding

                for(short i = (short)0; i < array.length; i++)
                    out.write((byte)(array[i] ? 1 : 0));
            }
            wasBeingSerialized = false;
        }
    }


    /**
     * Write a byte array.
     * @param array The byte array to write.
     * @param fieldId The id of the byte array field. Used for duplicate checking.
     * @throws ISOException An ISOException exception can be throw by a write method of an SyncOuput instance.
     * @throws UserException An UserException exception can be throw by a write method of an SyncOuput instance.
     * @see sync.SyncOutputCryptoAPDU#write(byte)
     * @see sync.SyncOutputCryptoAPDU#write(short)
     */
    public void write(byte[] array, short fieldId)
        throws ISOException, UserException {

        if(JCSystem.isTransient(array) == JCSystem.NOT_A_TRANSIENT_OBJECT) {
            if(!wasBeingSerialized) {
                isDuplicateReference = currentApplet.isArrayReferenceDuplicate(array, fieldId);
                wasBeingSerialized = true;
            }

            if(isDuplicateReference) {
                out.write(IS_DUPLICATE_REFERENCE);
                out.write(SyncApplet.duplicatedArrayReferenceInstanceId);
                out.write(SyncApplet.duplicatedArrayReferenceFieldId);
            } else {
                out.write(IS_UNIQUE_REFERENCE);
                out.write((short)array.length);
                out.write((short)-1); // padding

                for(short i = (short)0; i < array.length; i++)
                    out.write(array[i]);
            }
            wasBeingSerialized = false;
        }
    }


    /**
     * Write a short array.
     * @param array The short array to write.
     * @param fieldId The id of the short array field. Used for duplicate checking.
     * @throws ISOException An ISOException exception can be throw by a write method of an SyncOuput instance.
     * @throws UserException An UserException exception can be throw by a write method of an SyncOuput instance.
     * @see sync.SyncOutputCryptoAPDU#write(short)
     */
    public void write(short[] array, short fieldId)
        throws ISOException, UserException {
 
        if(JCSystem.isTransient(array) == JCSystem.NOT_A_TRANSIENT_OBJECT) {
            if(!wasBeingSerialized) {
                isDuplicateReference = currentApplet.isArrayReferenceDuplicate(array, fieldId);
                wasBeingSerialized = true;
            }

            if(isDuplicateReference) {
                out.write(IS_DUPLICATE_REFERENCE);
                out.write(SyncApplet.duplicatedArrayReferenceInstanceId);
                out.write(SyncApplet.duplicatedArrayReferenceFieldId);
            } else {
                out.write(IS_UNIQUE_REFERENCE);
                out.write((short)array.length);
                out.write((short)-1); // padding

                for(short i = (short)0; i < array.length; i++)
                    out.write(array[i]);
            }
            wasBeingSerialized = false;
        }
    }


    /**
     * Terminate the write process.
     * @throws ISOException An ISOException exception can be thrown by the done method of an SyncOutput instance.
     * @see sync.SyncOutputCryptoAPDU#done() 
     */
    void done()
        throws ISOException {

        ++resetId;
        out.done();

    }

}
