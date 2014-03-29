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

import synclib.SyncInput;
import synclib.SyncSW;

import javacard.framework.AID;
import javacard.framework.APDU;
import javacard.framework.APDUException;
import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;
import javacard.framework.UserException;
import javacard.framework.Util;

/**
 * This class implements SyncInput using the current apdu buffer as input and decrypt it beforehand.
 */
public class SyncInputCryptoAPDU implements SyncInput {

    /**
     * Track the current read position within the apdu buffer.
     */
    private short currentIndex = (short)0;

    /**
     * The number of bytes read from the apdu since the start of the read process. 
     */
    private short alreadyRead = (short)0;

    /**
     * Tell us if the current bytes should be skipped for real read or not.
     */
    private boolean shouldSkip = false;

    /**
     * The number of bytes skipped since the resume of the read process.
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
     * The total number of bytes read from the APDU (should be lower than cipher.getMaxAPDUBufferLength()).
     */
    private short totalNbBytes = (short)0;

    /**
     * The number of the current APDU being read.
     */
    private short currentNumber = (short)0;

    /**
     * Was the reading process paused?
     */
    private boolean wasPaused = false;

    /**
     * Where the reading process was paused and thus where we should start in the APDU buffer.
     */
    private short whereWasPaused = (short)0;

    /**
     * Have we saved the upper byte of a short?
     */
    private boolean hasSavedShortUp = false;

    /**
     * The saved upper byte of short coming from the previous APDU (if any).
     */
    private byte savedShortUp = (byte)0;

    /**
     * The current position in the buffer for the readN method.
     */
    private short readNBufferPosition = (short)0;

    /**
     * The current length of the buffer for the readN method.
     */
    private short readNBufferLength = (short)0;

    /**
     * The buffer used by the readN method to stock data between process method calls.
     */
    private byte readNBuffer[] = null;


    /**
     * Create an instance of this class which read the merge data decrypted by the given cipher.
     * @param cipher The cipher used to decrypt the stream.
     */
    SyncInputCryptoAPDU(CryptoAPDU cipher) {

        this.cipher = cipher;

        readNBuffer = JCSystem.makeTransientByteArray(cipher.getMaxAPDUBufferLength(), JCSystem.CLEAR_ON_DESELECT);
        Util.arrayFillNonAtomic(readNBuffer, (short)0, cipher.getMaxAPDUBufferLength(), (byte)0);

    }


    /**
     * Start the read process from the apdu buffer.
     * @param appletAID The AID of the applet which data are being merged.
     * @throws ISOException An ISOEXception is thrown when the decrypted data are invalid.
     */
    public void start(AID appletAID)
        throws ISOException {

        if(currentIndex != (short)ISO7816.OFFSET_CDATA)
            currentIndex = (short)ISO7816.OFFSET_CDATA;

        if(alreadyRead != (short)0)
            alreadyRead = (short)0;

        if(shouldSkip)
            shouldSkip = false;

        if(alreadySkipped != (short)0)
            alreadySkipped = (short)0;

        this.appletAID = appletAID;

        if(currentNumber != (short)1)
            currentNumber = (short)1;

        if(wasPaused)
            wasPaused = false;

        if(readNBufferPosition != (short)0)
            readNBufferPosition = (short)0;

        totalNbBytes = (short)(APDU.getCurrentAPDUBuffer()[ISO7816.OFFSET_LC] & 0x00FF);
        if(totalNbBytes != (short)0) {
            APDU.getCurrentAPDU().setIncomingAndReceive();
            if(!cipher.decryptAPDU(ISO7816.OFFSET_CDATA, totalNbBytes))
                ISOException.throwIt(ISO7816.SW_DATA_INVALID);
        }

        totalNbBytes += ISO7816.OFFSET_CDATA;

        if(!checkAID())
            ISOException.throwIt(ISO7816.SW_DATA_INVALID);

        if(!checkNumber())
            ISOException.throwIt(ISO7816.SW_DATA_INVALID);

    }


    /**
     * Pause the reading process so as to resume it later at the same place in the APDU buffer.
     * @throws UserException An UserException exception is thrown to signal the need to replay this APDU.
     * @see SyncInput#REPLAY_THIS_APDU
     */
    public void replay()
        throws UserException {

        wasPaused = true;
        whereWasPaused = currentIndex;
        ++currentNumber;
        UserException.throwIt(SyncInput.REPLAY_THIS_APDU);   /* Requesting to replay this APDU */

    }


    /**
     * Resume the read process from the apdu buffer.
     * @throws ISOException An ISOEXception is thrown when the decrypted data are invalid.
     */
    public void resume()
        throws ISOException {

        if(currentIndex != (short)ISO7816.OFFSET_CDATA)
            currentIndex = (short)ISO7816.OFFSET_CDATA;

        if(alreadyRead == (short)0) {
            if(shouldSkip)
                shouldSkip = false;
        } else {
            if(!shouldSkip)
                shouldSkip = true;
        }

        if(alreadySkipped != (short)0)
            alreadySkipped = (short)0;

        totalNbBytes = (short)(APDU.getCurrentAPDUBuffer()[ISO7816.OFFSET_LC] & 0x00FF);
        if(totalNbBytes != (short)0) {
            APDU.getCurrentAPDU().setIncomingAndReceive();
            if(!cipher.decryptAPDU(ISO7816.OFFSET_CDATA, totalNbBytes))
                ISOException.throwIt(ISO7816.SW_DATA_INVALID);
        } else
            ISOException.throwIt(ISO7816.SW_DATA_INVALID);

        totalNbBytes += ISO7816.OFFSET_CDATA;

        if(!checkAID())
            ISOException.throwIt(ISO7816.SW_DATA_INVALID);

        if(!checkNumber())
            ISOException.throwIt(ISO7816.SW_DATA_INVALID);

        if(wasPaused) {
            currentIndex = whereWasPaused;
            wasPaused = false;
        }

    }


    /**
     * Reset the resume process as to avoid unneeded ghost readings.
     */
    public void resetResume() {

        if(alreadyRead != (short)0)
            alreadyRead = (short)0;

    }


    /**
     * Check if the AID within the APDU is the same than the applet being merged one.
     * @return Return true if the AIDs match, false else.
     */
    private boolean checkAID() {

        byte[] buffer = APDU.getCurrentAPDUBuffer();
        currentIndex = (short)(ISO7816.OFFSET_CDATA + 1 + (buffer[ISO7816.OFFSET_CDATA] & 0x00FF));
        return appletAID.equals(buffer, (short)(ISO7816.OFFSET_CDATA + 1), buffer[ISO7816.OFFSET_CDATA]);

    }


    /**
     * Check the APDU is the expected one in the sequence of APDUs.
     * @return Return true if the number is the expected one, false else.
     */
    private boolean checkNumber() {

        byte[] buffer = APDU.getCurrentAPDUBuffer();
        short readNumber = Util.makeShort(buffer[currentIndex], buffer[(short)(currentIndex + (short)1)]);
        currentIndex += 2;
        return readNumber == currentNumber;

    }


    /**
     * Read a byte from the current apdu buffer and return it.
     * @param b The current value of the read byte.
     * @return The byte value read.
     * @throws UserException An UserException is thrown if the APDU was fully read.
     * @see SyncInput#WAITING_FOR_MORE
     */
    public byte read(byte b)
        throws UserException {

        if(totalNbBytes == currentIndex) {
            currentNumber++;
            UserException.throwIt(SyncInput.WAITING_FOR_MORE);   // Requesting more data
        }

        if(shouldSkip) {
            alreadySkipped++;
            if(alreadySkipped == alreadyRead)
                shouldSkip = false;
        } else {
            byte[] buffer = APDU.getCurrentAPDUBuffer();
            b = buffer[currentIndex];
            currentIndex++;
            alreadyRead++;
        }

        return b;

    }


    /**
     * Read a short from the current apdu buffer and return it.
     * @param s The current value of the read short.
     * @return The short value read.
     * @throws UserException An UserException is thrown if the APDU was fully read. 
     * @see SyncInput#WAITING_FOR_MORE
     */
    public short read(short s)
        throws UserException {

        byte b1 = (byte)(s >> 8);
        byte b2 = (byte)(s & 0xFF);

        b1 = read(b1);
        if(hasSavedShortUp && !shouldSkip) {
            b1 = savedShortUp;
            hasSavedShortUp = false;
        }

        try {
            b2 = read(b2);
        }
        catch(UserException e) {
            hasSavedShortUp = true;
            savedShortUp = b1;
            throw e;
        }

        return Util.makeShort(b1, b2);

    }


    /**
     * Read n bytes to access them as an array.
     * @param n The number of bytes to read; should be less than the result from getMaxAPDUBufferLength method.
     * @return Return True if the reading was just completed, false if the reading was a ghost one.
     * @throws ISOException An ISOException exception is thrown if n is too big.
     * @throws UserException An UserException is thrown if the APDU was fully read. 
     * @see ISO7816#SW_UNKNOWN
     * @see SyncInput#WAITING_FOR_MORE
     */
    public boolean readN(short n)
        throws ISOException, UserException {

        if(n > cipher.getMaxAPDUBufferLength())
            ISOException.throwIt(ISO7816.SW_UNKNOWN);

        if(totalNbBytes == currentIndex) {
            currentNumber++;
            UserException.throwIt(SyncInput.WAITING_FOR_MORE);   // Requesting more data
        }

        if(shouldSkip) {
            //If the reading request was already completed, we skip it entirely.
            if((short)(alreadySkipped + n) <= alreadyRead) {
                alreadySkipped += n;

                if(alreadySkipped == alreadyRead)
                    shouldSkip = false;

                return false;
            }

            //Else we complete it.
            alreadySkipped = alreadyRead;
            shouldSkip = false;
        }

        byte[] buffer = APDU.getCurrentAPDUBuffer();
        short nbBytesToCopy = ((short)(n - readNBufferPosition)) < (short)((totalNbBytes - currentIndex)) ? (short)(n - readNBufferPosition) : (short)(totalNbBytes - currentIndex);

        Util.arrayCopyNonAtomic(buffer, currentIndex, readNBuffer, readNBufferPosition, nbBytesToCopy);

        alreadyRead += nbBytesToCopy;
        readNBufferPosition += nbBytesToCopy;

        if(readNBufferPosition != n) {
            currentNumber++;
            UserException.throwIt(SyncInput.WAITING_FOR_MORE);
        }

        currentIndex += nbBytesToCopy;

        readNBufferPosition = (short)0;
        readNBufferLength = n;

        return true;

    }


    /**
     * Get a value read by the readN method.
     * @param i The index of the value to read.
     * @return The i-th read value.
     * @throws ISOException An ISOException exception is thrown if i is bigger than the parameter n given in the last call of the readN method.
     * @see ISO7816#SW_UNKNOWN
     */
    public byte getReadNValue(short i)
        throws ISOException {

        if(i >= readNBufferLength)
            ISOException.throwIt(ISO7816.SW_UNKNOWN);

        return readNBuffer[i];

    }


    /**
     * Terminate the read process from the apdu buffer.
     */
    public void done() {
        Util.arrayFillNonAtomic(readNBuffer, (short)0, cipher.getMaxAPDUBufferLength(), (byte)0);
    }

}
