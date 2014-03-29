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

import javacard.framework.Util;
import javacard.framework.JCSystem;
import javacard.framework.APDU;
import javacard.security.AESKey;
import javacard.security.Key;
import javacard.security.KeyBuilder;
import javacard.security.RandomData;
import javacardx.crypto.Cipher;
import javacard.security.CryptoException;

/**
 * This class implements CryptoAPDU using AES 128bits as cipher.
 */
class AES128CryptoAPDU implements CryptoAPDU {

    /**
     * The minimum size of the apdu buffer leaving out the header.
     */
    private static final short maxAPDUBufferLength = (short)32;//128;

    /**
     * Block size.
     */
    private static final short blockSize = (short)16;

    /**
     * Temporary block for encryption purpose.
     */
    private byte[] block = null;

    /**
     * The cipher instance.
     */
    private Cipher cipher = null;

    /**
     * The random generator instance used for padding data.
     */
    private RandomData generator = null;

    /**
     * The current key used for encryption/decryption.
     */
    private AESKey key = null;

    /**
     * The current initial vector used for encryption/decryption.
     */
    private byte[] initialVector = null;


    /**
     * Create an instance of the APDU cipher.
     * @param generator The random generator used for padding the last block.
     */
    AES128CryptoAPDU(RandomData generator) {

        block = JCSystem.makeTransientByteArray(blockSize, JCSystem.CLEAR_ON_DESELECT);
        cipher = Cipher.getInstance(Cipher.ALG_AES_BLOCK_128_CBC_NOPAD, false);
        this.generator = generator;

    }


    /**
     * Change the AES128 cipher key and initial vector.
     * @param key The AES key.
     * @param bArray The AES initial vector.
     */
    public void init(Key key, byte bArray[]) {

        this.key = (AESKey)key;
        initialVector = bArray;

    }  


    /**
     * Encrypt the current APDU buffer starting from offset and going on for len bytes.
     * @param offset Where should the encryption start from in the current APDU buffer.
     * @param len The length of the section to encrypt.
     * @return Return the number of bytes encrypted rounded up to the block size of the cipher or return -1 in case of error.
     */
    public short encryptAPDU(short offset, short len) {

        byte[] buffer = APDU.getCurrentAPDUBuffer();

        if(len > maxAPDUBufferLength)
            return (short)-1;

        if(len == (short)0)
            return (short)0;

        short remainder = (short)(len % blockSize);

        if(remainder != 0) {
            remainder = (short)(blockSize - remainder);
            generator.generateData(buffer, (short)(len + offset), remainder);
//            for(short i = 0; i < remainder; ++i)
//                buffer[(short)(len + offset + i)] = (byte)-1;
        }

        short newLength = (short)(len + remainder);
        short nbBlock = (short)(newLength / blockSize);

        cipher.init(key, Cipher.MODE_ENCRYPT, initialVector, (short)0, (short)16);

        for(short i = 0; i < nbBlock; i++) {
            Util.arrayCopyNonAtomic(buffer, (short)((blockSize * i) + offset), block, (short)0, blockSize);
            cipher.update(block, (short)0, blockSize, buffer, (short)((blockSize * i) + offset));
        }

        return newLength;

    }


    /**
     * Decrypt the current APDU buffer starting from offset and going on for len bytes.
     * @param offset Where should the decryption start from in the current APDU buffer.
     * @param len The length of the section to decrypt.
     * @return Return true if the decryption is successful, false else.
     */
    public boolean decryptAPDU(short offset, short len) {

        byte[] buffer = APDU.getCurrentAPDUBuffer();

        if(len > maxAPDUBufferLength)
            return false;

        if(len == (short)0)
            return true;

        if((len % blockSize) != 0)
            return false;

        short nbBlock = (short)(len / blockSize);

        cipher.init(key, Cipher.MODE_DECRYPT, initialVector, (short)0, (short)16);

        for(short i = 0; i < nbBlock; i++) {
            Util.arrayCopyNonAtomic(buffer, (short)((blockSize * i) + offset), block, (short)0, blockSize);
            cipher.update(block, (short)0, blockSize, buffer, (short)((blockSize * i) + offset));
        }

        return true;

    }


    /**
     * Return the maximum usable length in the APDU buffer (usually 128bytes).
     * @return The length.
     */
    public short getMaxAPDUBufferLength() {

        return maxAPDUBufferLength;

    }

}
