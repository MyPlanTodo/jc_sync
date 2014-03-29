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

/**
 * This interface regroups constants representing the response status used within sync.
 */
public interface SyncSW {

    /**
     * This response status is used when the provided number of managed AIDs is wrong.
     */
    public static final short WRONG_NB_AIDS            = (short)0x4201;

    /**
     * This response status is used when the provided id is out of range.
     */
    public static final short WRONG_ID                 = (short)0x4202;

    /**
     * This response status is used when the expected caller's AID is not the one defined as sync.
     */
    public static final short IS_NOT_sync               = (short)0x4203;

    /**
     * This response status is used when an APDUException was caught.
     */
    public static final short APDU_EXCEPTION           = (short)0x4204;

    /**
     * This response status is used when the requested AID is not managed by sync.
     */
    public static final short AID_NOT_FOUND            = (short)0x4205;

    /**
     * This response status is used when the requested AID is not known by the VM.
     */
    public static final short APPLET_NOT_FOUND         = (short)0x4206;

    /**
     * This response status is used when the id of the continue instruction is not the one expected.
     */
    public static final short WRONG_ID_RESUME          = (short)0x4208;

    /**
     * This response status is used when a continue merge instruction was expected and a serialize instruction was received.
     */
    public static final short MERGE_INS_EXPECTED       = (short)0x4209;

    /**
     * This response status is used when a continue serialize instruction was expected and a merge instruction was received.
     */
    public static final short SERIALIZE_INS_EXPECTED   = (short)0x4210;

    /**
     * This response status is used when an instruction was not expected.
     */
    public static final short INS_NOT_EXPECTED         = (short)0x4211;

    /**
     * This response status is used when the applet is in an incoherent state and must be synchronized.
     */
    public static final short INCOMPLETE_MERGE         = (short)0x4215;

    /**
     * This response status is used when the sync device is not yet configured and the used instruction can not be used in that context.
     */
    public static final short NOT_CONFIGURED           = (short)0x4216;

    /**
     * This response status is used when the current challenge process failed.
     */
    public static final short CHALLENGE_FAILED         = (short)0x4217;

    /**
     * This response statis is used when the current merging applet is done.
     */
    public static final short MERGE_DONE               = (short)0x4219;

}
