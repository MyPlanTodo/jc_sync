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

public interface SynchronizableArrayFields extends Synchronizable {

    /**
     * Return the reference to the next Synchronizable instance in the linked list.
     * @return Return the reference.
     */
    SynchronizableArrayFields getNext();

    /**
     * Set the reference to the next Synchronizable instance in the linked list.
     * @param next The next Synchronizable instance.
     */
    void setNext(SynchronizableArrayFields next);

    /**
     * Return the number of boolean arrays in this Synchronizable instance.
     * @return Return the number of boolean arrays.
     */
    short getNbBooleanArrays();

    /**
     * Return a boolean array given its field id in this Synchronizable instance.
     * @param fieldId The id of the boolean array.
     * @return Return the boolean array.
     * @throws ISOException An ISOException exception is thrown if the field id is invalid.
     * @see ISO7816#SW_UNKNOWN
     */
    boolean[] getBooleanArray(short fieldId) throws ISOException;

    /**
     * Return the number of byte arrays in this Synchronizable instance.
     * @return Return the number of byte arrays.
     */
    short getNbByteArrays();

    /**
     * Return a byte array given its field id in this Synchronizable instance.
     * @param fieldId The id of the byte array.
     * @return Return the byte array.
     * @throws ISOException An ISOException exception is thrown if the field id is invalid.
     * @see ISO7816#SW_UNKNOWN
     */
    byte[] getByteArray(short fieldId) throws ISOException;

    /**
     * Return the number of short arrays in this Synchronizable instance.
     * @return Return the number of short arrays.
     */
    short getNbShortArrays();

    /**
     * Return a short array given its field id in this Synchronizable instance.
     * @param fieldId The id of the short array.
     * @return Return the short array.
     * @throws ISOException An ISOException exception is thrown if the field id is invalid.
     * @see ISO7816#SW_UNKNOWN
     */
    short[] getShortArray(short fieldId) throws ISOException;

}
