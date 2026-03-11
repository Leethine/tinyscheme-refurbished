! $Id: vaxc.com,v 1.6 2021/09/20 02:45:22 alex Exp $
!*******************************************************************************
!
!    Compile and create general purpose library.
!
!    This DCL command file will build the library using the old VAX C
!    compiler.  A source file is recompiled only if its revision time
!    is newer than that of its object file.
!
!    I haven't had access to a VAX since the mid-2000s and I've never
!    had access to the newer DEC C compiler.  I've added a few source
!    files since then and I hope they build correctly.  Let me know
!    if they don't!
!
!*******************************************************************************
$
$ sources =	-
	"aperror,"	+-
	"bio_util,"	+-
	"bit_util,"	+-
	"bmw_util,"	+-
	"coli_util,"	+-
	"comx_util,"	+-
	"cosn_util,"	+-
	"crlf_util,"	+-
	"drs_util,"
$ sources = sources	+-
	"f1750a_util,"	+-
	"ffs,"		+-
	"fnm_util,"	+-
	"get_util,"	+-
	"gimx_util,"	+-
	"gsc_util,"	+-
	"hash_util,"
$ sources = sources	+-
	"id3_util,"	+-
	"ieee_util,"	+-
	"iiop_util,"	+-
	"ioctl_ucx,"	+-
	"iox_util,"	+-
	"lfn_util,"	+-
	"list_util,"	+-
	"log_util,"	+-
	"meo_util,"
$ sources = sources	+-
	"nbr_util,"	+-
	"nbr_util_primes,"	+-
	"nbr_util_roots,"	+-
	"net_util,"	+-
	"nft_proc,"	+-
	"nft_util,"	+-
	"nnl_util,"	+-
	"nvl_util,"	+-
	"nvp_util,"	+-
	"opt_util,"
$ sources = sources	+-
	"port_util,"	+-
	"rex_internals,"	+-
	"rex_util,"	+-
	"rex_util_y,"	+-
	"sdx_util,"	+-
	"skt_util,"	+-
	"srt_util,"	+-
	"sto_util,"	+-
	"str_util,"
$ sources = sources	+-
	"tcp_util,"	+-
	"tpl_util,"	+-
	"ts_util,"	+-
	"tv_util,"	+-
	"udp_util,"	+-
	"utf_util,"	+-
	"vim_util,"	+-
	"wcs_util,"	+-
	"xdr_util,"	+-
	"xnet_util,"	+-
	"xqt_util"
$ objectlib = "libgpl.olb"
$
$ echo := write sys$output
$
$ define sys sys$library
$ define arpa sys$library
$ define netinet sys$library
$ rpcinc = f$parse ("[-.include.rpc]")
$ define rpc 'f$extract (0, f$locate ("]", rpcinc) + 1, rpcinc)'
$
$ command =						-
	"cc/debug=all/opt"				+-
	"/include=([-.include],[-.include.vms])"	+-
	"/define=(""""""__vax__"""""")"
$ echo "''command'"
$ compile := 'command'
$ show symbol compile
$
$ i = 0
$ NextFile:
$     file = f$element (i, ",", sources)
$     if (file .nes. ",")
$     then
$         srev = f$file_attributes (file + ".c", "RDT")
$         orev = f$file_attributes (file + ".obj", "RDT")
$         if (.not. $status) then orev = " "
$         if (srev .gts. orev)
$         then
$             echo "Compiling ''file' ..."
$             compile 'file'
$         else
$             echo "Up-to-date ''file' ..."
$         endif
$         i = i + 1
$         goto NextFile
$     endif
$
$ CreateLibrary:
$
$ echo "Creating ''objectlib' ..."
$ library/create/list 'objectlib' 'sources'
