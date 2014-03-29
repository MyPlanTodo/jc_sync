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

package dock;

import java.io.InputStream;
import java.io.FileInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.HashMap;

import java.math.BigInteger;

/**
 * This class implements an applet library using local CAP file added to it manually.
 */
public class LocalAppletLibrary implements AppletLibrary {

    /**
     * The list of CAP files path mapped by AIDs.
     */
    private HashMap<BigInteger, File>list;

    /**
     * Create an empty local applet library.
     */
    public LocalAppletLibrary() {

        list = new HashMap<BigInteger, File>();

    }


    /**
     * Add a CAP file and its AID to the library given a path to the file.
     * @param AID The AID of the applet.
     * @param pathToCAPFile The path to the CAP file to add.
     */ 
    public void add(byte[] AID, String pathToCAPFile) {

        list.put(new BigInteger(AID), new File(pathToCAPFile));

    }


    /**
     * Search the library for an applet CAP file given its AID and return an InputStream.
     * @param AID The AID of the applet.
     * @return An InputStream to the CAP file.
     * @throws AppletNotFoundException An AppletNotFoundException is thrown if the requested applet is not found within the library.
     */
    public InputStream getAppletCAPByAID(byte[] AID)
        throws AppletNotFoundException {

        BigInteger aid = new BigInteger(AID);

        if(!list.containsKey(aid))
            throw new AppletNotFoundException("The requested applet is not present within the library");

        File CAPFile = list.get(aid);
        InputStream input = null;
        try {
            input = (InputStream)new FileInputStream(CAPFile);
        }
        catch(FileNotFoundException e) {
            throw new AppletNotFoundException("The requested applet is not present within the library");
        }

        return input;

    }


    /**
     * Search for an applet in the library being given an AID and give its CAP file size.
     * @param appletAID The searched for applet's AID.
     * @return The applet CAP file size.
     * @throws AppletNotFoundException An AppletNotFoundException is thrown if the searched for applet is not found within the library.
     */
    public long getAppletCAPSizeByAID(byte[] appletAID)
        throws AppletNotFoundException {
        BigInteger aid = new BigInteger(appletAID);

        if(!list.containsKey(aid))
            throw new AppletNotFoundException("The requested applet is not present within the library");

        File CAPFile = list.get(aid);
        return CAPFile.length();

    }

}
