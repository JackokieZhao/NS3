set(name wifi-phy-test)
set(source_files ${name}.cc)
set(header_files)
set(libraries_to_link ${libcore} ${libmobility} ${libnetwork} ${libwifi})
set(files_to_copy)
build_lib_example ("${name}" "${source_files}" "${header_files}" "${libraries_to_link}" "${files_to_copy}")


set(name wifi-test-interference-helper)
set(source_files ${name}.cc)
set(header_files)
set(libraries_to_link ${libcore} ${libmobility} ${libnetwork} ${libwifi})
set(files_to_copy)
build_lib_example ("${name}" "${source_files}" "${header_files}" "${libraries_to_link}" "${files_to_copy}")


set(name wifi-manager-example)
set(source_files ${name}.cc)
set(header_files)
set(libraries_to_link ${libcore} ${libnetwork} ${libwifi} ${libstats} ${libmobility} ${libpropagation})
set(files_to_copy)
build_lib_example ("${name}" "${source_files}" "${header_files}" "${libraries_to_link}" "${files_to_copy}")


set(name wifi-trans-example)
set(source_files ${name}.cc)
set(header_files)
set(libraries_to_link ${libcore} ${libnetwork} ${libwifi} ${libstats} ${libmobility} ${libpropagation})
set(files_to_copy)
build_lib_example ("${name}" "${source_files}" "${header_files}" "${libraries_to_link}" "${files_to_copy}")


set(name wifi-phy-configuration)
set(source_files ${name}.cc)
set(header_files)
set(libraries_to_link ${libwifi} ${libconfig-store})
set(files_to_copy)
build_lib_example ("${name}" "${source_files}" "${header_files}" "${libraries_to_link}" "${files_to_copy}")


set(name wifi-bianchi)
set(source_files ${name}.cc)
set(header_files)
set(libraries_to_link ${libwifi} ${libapplications} ${libinternet-apps})
set(files_to_copy)
set(ignore_example 1)
build_lib_example ("${name}" "${source_files}" "${header_files}" "${libraries_to_link}" "${files_to_copy}")