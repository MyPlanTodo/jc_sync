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

package sync;

import synclib.SyncOutput;

import javacard.framework.AID;
import javacard.framework.APDU;
import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.UserException;
import javacard.framework.Util;

/**
 * This class implements SyncOuput using the current apdu buffer as output and encrypt it.
 */
public class SyncOutputCryptoAPDU implements SyncOutput {

    /**
     * Track the current write position within the apdu buffer.
     */
    private short currentIndex = (short)0;

    /**
     * The number of bytes written through an apdu since the start of the write process.
     */
    private short alreadyWritten = (short)0;

    /**
     * Tell us if the current byte should be skipped for real write or not.
     */
    private boolean shouldSkip = false;

    /**
     * The number fo bytes skipped since the resume of the write process.
     */
    private short alreadySkipped = (short)0;

    /**
     * The AID of the current applet serialization stream.
     */
    private AID appletAID = null;

    /**
     * The length of the current applet AID.
     */
    private short AIDLength = (short)0;

    /**
     * The cipher used to encrypt the serialization stream.
     */
    private CryptoAPDU cipher = null;

    /**
     * The maximum usable length in the APDU buffer incurred by the use of a particular cipher.
     */
    private short maxAPDUBufferLength = (short)0;

    /**
     * The number of the current APDU being writen.
     */
    private short currentNumber = (short)0;

    /**
     * Store the last generated APDU buffer for reuse in case of replay.
     */
    private byte[] lastBuffer = null;


    /**
     * Create an instance of this class which stream out the serialized data encrypted by the given cipher.
     * @param cipher The cipher used to encrypt the stream.
     */
    SyncOutputCryptoAPDU(CryptoAPDU cipher) {

        this.cipher = cipher;
        maxAPDUBufferLength = cipher.getMaxAPDUBufferLength();
        lastBuffer = new byte[maxAPDUBufferLength];

    }


    /**
     * Start the write process to the apdu buffer.
     * @param appletAID The AID of the applet which serialized data are streamed out.
     */
    public void start(AID appletAID) {

        if(currentIndex != (short)0)
            currentIndex = (short)0;

        if(alreadyWritten != (short)0)
            alreadyWritten = (short)0;

        if(shouldSkip)
            shouldSkip = false;

        if(alreadySkipped != (short)0)
            alreadySkipped = (short)0;

        this.appletAID = appletAID;
        writeAID();
        AIDLength = (short)(currentIndex - 1);

        if(currentNumber != (short)1)
            currentNumber = (short)1;
        writeNumber();

    }


    /**
     * Resume the write process to the apdu buffer.
     */
    public void resume() {

        if(currentIndex != (short)0)
            currentIndex = (short)0;

        if(alreadyWritten == (short)0) {
            if(shouldSkip)
                shouldSkip = false;
        } else {
            if(!shouldSkip)
                shouldSkip = true;
        }

        if(alreadySkipped != (short)0)
            alreadySkipped = (short)0;

        writeAID();
        writeNumber();

    }


    /** 
     * Replay the last generated APDU buffer.
     * @throws ISOException An ISOException is thrown if the encryption process failled.
     * @throws UserException An UserException is thrown when the apdu buffer is filled with the last generated APDU buffer.
     * @see ISO7816#SW_DATA_INVALID
     */
    public void replay()
        throws ISOException, UserException {

        lastBuffer[(short)(lastBuffer[0] + 1)] = (byte)(currentNumber >> 8);
        lastBuffer[(short)(lastBuffer[0] + 2)] = (byte)(currentNumber & 0xFF);
        Util.arrayCopyNonAtomic(lastBuffer, (short)0, APDU.getCurrentAPDUBuffer(), (short)0, currentIndex);

        short length = cipher.encryptAPDU((short)0, currentIndex);
        if(length == -1)
            ISOException.throwIt(ISO7816.SW_DATA_INVALID);
        else {
            currentNumber++;
            UserException.throwIt(length);
        }

    }

    /**
     * Write the AID in the APDU buffer.
     */
    private void writeAID() {

        byte[] buffer = APDU.getCurrentAPDUBuffer();
        buffer[0] = appletAID.getBytes(buffer, (short)1);
        lastBuffer[0] = appletAID.getBytes(lastBuffer, (short)1);
        currentIndex = (short)((buffer[0] & 0x00FF) + 1);

    }


    /**
     * Write the sequence number of the processed APDU.
     */
    private void writeNumber() {

        byte[] buffer = APDU.getCurrentAPDUBuffer();
        buffer[currentIndex] = (byte)(currentNumber >> 8);
        buffer[(short)(currentIndex + 1)] = (byte)(currentNumber & 0xFF);

        lastBuffer[currentIndex] = buffer[currentIndex];
        lastBuffer[(short)(currentIndex + 1)] = buffer[(short)(currentIndex + 1)];

        currentIndex += 2;

    }


    /**
     * Reset the resume process as to avoid unneeded ghost writings.
     */
    public void resetResume() {

        if(alreadyWritten != (short)0)
            alreadyWritten = (short)0;

    }


    /**
     * Write a byte to the current apdu buffer.
     * @param b The byte to write.
     * @throws ISOException An ISOException is thrown if the encryption process failled.
     * @throws UserException An UserException is thrown when the apdu buffer is full.
     * @see ISO7816#SW_DATA_INVALID
     */
    public void write(byte b)
        throws ISOException, UserException {

        if(currentIndex == maxAPDUBufferLength) {
            short length = cipher.encryptAPDU((short)0, currentIndex);
            if(length == -1)
                ISOException.throwIt(ISO7816.SW_DATA_INVALID);
            else {
                currentNumber++;
                UserException.throwIt(length);
            }
        }

        if(shouldSkip) {
            alreadySkipped++;
            if(alreadySkipped == alreadyWritten)
                shouldSkip = false;
        } else {
            byte[] buffer = APDU.getCurrentAPDUBuffer();
            buffer[currentIndex] = b;
            lastBuffer[currentIndex] = b;
            currentIndex++;
            alreadyWritten++;
        }

    }


    /**
     * Write a short to the current apdu buffer.
     * @param s The short to write.
     * @throws ISOException An ISOException is thrown if the encryption process failled.
     * @throws UserException An UserException is thrown when the apdu buffer is full.
     * @see ISO7816#SW_DATA_INVALID
     */
    public void write(short s)
        throws ISOException, UserException {

        write((byte)(s >> 8));
        write((byte)s);

    }


    /**
     * Cipher the apdu buffer and wrap up the writing process.
     * @throws ISOException An ISOException is thrown if the encryption failled.
     * @see ISO7816#SW_DATA_INVALID
     */
    public void done()
        throws ISOException {

        short length = cipher.encryptAPDU((short)0, currentIndex);
        if(length == -1)
            ISOException.throwIt(ISO7816.SW_DATA_INVALID);
        APDU apdu = APDU.getCurrentAPDU();
        apdu.setOutgoingAndSend((short)0, length);

    }

}
