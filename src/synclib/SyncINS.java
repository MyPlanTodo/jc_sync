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
 * This interface regroups constants representing the supported instructions of the syncManager as well as the supported class.
 */
public interface SyncINS {

    /**
     * The class in which the instructions should be received.
     */
    public static final byte CLA = (byte)0x80;

    /**
     * This instruction returns through apdu the list of managed AIDs.
     */
    public static final byte LIST_AIDS              = (byte)0x00;

    /**
     * This instruction returns through apdu the number of managed AIDs.
     */
    public static final byte GET_NB_AIDS            = (byte)0x02;

    /**
     * This instruction returns through apdu the AID given an id.
     */
    public static final byte GET_AID_BY_ID          = (byte)0x04;

    /**
     * This instruction returns through apdu the serialized data from a managed applet given an id.
     */
    public static final byte SERIALIZE_APPLET_BY_ID = (byte)0x06;

    /**
     * This instruction returns the id of a managed AID given an AID.
     */
    public static final byte CHECK_INSTALL          = (byte)0x08;

    /**
     * This instruction starts the merge process given an id.
     */
    public static final byte MERGE_APPLET_BY_ID     = (byte)0x0A;

    /**
     * This instruction resumes the serialization process of an applet given its id.
     */
//    public static final byte RESUME_SERIALIZATION = (byte)0x0C;

    /**
     * This instruction resumes the merge process of an applet given its id.
     */
//    public static final byte RESUME_MERGE = (byte)0x0E;

    /**
     * This instruction replays the last serialization APDU.
     */
//    public static final byte REPLAY_LAST_APDU = (byte)0x26;

    /**
     * This instruction starts the configuration process for a new sync device.
     */
    public static final byte START_CONFIG = (byte)0x10;

    /**
     * This instruction starts the authentication process of an sync device by asking a challenge.
     */
    public static final byte GET_SERVER_CHALLENGE = (byte)0x12;

    /**
     * This instruction asks a challenge to the wannabe sync device.
     */
    public static final byte ANSWER_SERVER_CHALLENGE = (byte)0x14;

    /**
     * This instruction finalizes the authentication process between the sync devices.
     */
    public static final byte ANSWER_CLIENT_CHALLENGE = (byte)0x16;

    /**
     * This instruction asks for fingerprint data from an sync device for the sync device authenticated with it.
     */
    public static final byte GET_FINGERPRINT = (byte)0x18;

    /**
     * This instruction checks for the provided fingerprint data and gives the userkey in return.
     */
    public static final byte GET_USERKEY = (byte)0x1A;

    /**
     * This instruction installs the userkey within a new sync device.
     */
    public static final byte INSTALL_USERKEY = (byte)0x1C;

    /**
     * This instruction aborts a challenge process (from a configuration process for example).
     */
    public static final byte ABORT_CHALLENGE = (byte)0x1E;

    /**
     * This instruction asks if the sync device is configured (i.e. has an userkey).
     */
    public static final byte IS_CONFIGURED = (byte)0x20;

    /*
     * TODO: To remove
     */
    public static final byte DUMMY_INSTALL_USERKEY = (byte)0x22;

    /*
     * TODO: To remove
     */
    public static final byte DUMMY_SET_SEED = (byte)0x24;

    public static final byte START_SERIALIZE_SESSION = (byte)0x28;

    public static final byte START_MERGE_SESSION = (byte)0x2A;

    public static final byte END_SESSION = (byte)0x2C;

}
