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
import javacard.framework.UserException;

public interface Synchronizable {

    /**
     * Reset the path supporting fields to their default values.
     * @return Return true if this Synchronizable instance fields were reset, false else.
     */
    public boolean resetPath(ResetIdSource source);

    /**
     * Number an instance with an id for duplicate instances detection.
     * @return Return true if this Synchronizable instance was numbered, false else.
     */
    public boolean numberPath(Output out);


    /**
     * Return the id of this Synchronizable instance.
     * @return Return the id.
     */
    public short getId();

    /**
     * Set the id of this Synchronizable instance.
     * @param id The id to set.
     */
    public void setId(short id);

    /**
     * Return the duplicate state.
     * @return Return true if this Synchronizable instance is duplicated, false else.
     */
    public boolean isDuplicated();


    /**
     * Set the previously visited Synchronizable instance for later backtracking.
     * @param previous The previously visited Synchronizable instance.
     */
    public void setPreviousSynchronizableInstance(Synchronizable previous);

    /**
     * Get the previously visited Synchronizable instance for backtracking to it.
     * @return Return the previously visited Synchronizable instance.
     */
    public Synchronizable getPreviousSynchronizableInstance();


    /**
     * Serialize primitive type and array of primitive type fields.
     * @param out The sink for the output.
     * @throws ISOException An ISOException exception can be thrown be a write method of Output.
     * @throws UserException An UserException exception can be thrown be a write method of Output.
     * @see Output#write(boolean)
     * @see Output#write(byte)
     * @see Output#write(short)
     * @see Output#write(boolean[], short)
     * @see Output#write(byte[], short)
     * @see Output#write(short[], short)
     */
    void serialize(Output out) throws ISOException, UserException;

    /**
     * Merge primitive type and array of primitive type fields.
     * @param in The source for the input.
     * @throws ISOException An ISOException exception can be thrown be a read or checkArray method of Input.
     * @throws UserException An UserException exception can be thrown be a read or checkArray method of Input.
     * @see Input#read(boolean)
     * @see Input#read(byte)
     * @see Input#read(short)
     * @see Input#checkArray(boolean[])
     * @see Input#read(boolean[])
     * @see Input#checkArray(byte[])
     * @see Input#read(byte[])
     * @see Input#checkArray(short[])
     * @see Input#read(short[])
     */
    void merge(Input in) throws ISOException, UserException;

}
