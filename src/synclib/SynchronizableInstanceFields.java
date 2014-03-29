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

public interface SynchronizableInstanceFields extends Synchronizable {

    /**
     * Indicate if there is a currently selected Synchronizable instance.
     * @return Return true if this Synchronizable instance has a currently selected Synchronizable instance fields or super fields.
     */
    public boolean hasCurrentSynchronizableInstance();

    /**
     * Select the next Synchronizable instance field or super field within this Synchronizable instance.
     */
    public void selectNextSynchronizableInstance();

    /**
     * Return the currently selected Synchronizable instance field or super field of this Synchronizable instance.
     * @return Return the currently selected field of super field. Can be null.
     * @throws ClassCastException A ClassCastException exception is thrown if the current instance is not a Synchronizable one and should be skipped. The exception will be catched by the SyncApplet.serializeAll method.
     */ 
    public Synchronizable getCurrentSynchronizableInstance() throws ClassCastException;

    /**
     * Set the currently selected Synchronizable instance field of this Synchronizable instance.
     * @param instance The Synchronizable instance to be set as value of the currently selected Synchronizable instance field.
     */
    public void setCurrentSynchronizableInstance(Synchronizable instance);

    /**
     * Reset the selection process to its starting state.
     */
    public void resetNextSynchronizableInstanceSelect();

}
