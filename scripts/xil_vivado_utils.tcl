## ----------------------------------------------------------------------------
## 
## ----------------------------------------------------------------------------
namespace eval ::roe::util {

  proc ifDirNotExistsCreate { dirPath } {
    if { ! [file exists $dirPath] } {
      file mkdir $dirPath
    }
  }

  proc xroe_matchProjectProp {prop matchString} {
    set currentBoard [get_property $prop [get_projects [current_project]]]
    if {[regexp $matchString $currentBoard] == 1} {
      return true
    } else {
      return false
    }
  }

  proc xroe_checkBoardExists {} {
    return [xroe_matchProjectProp BOARD_PART ".+"]
  }

  proc xroe_checkBoardPart {matchString} {
    return [xroe_matchProjectProp BOARD_PART $matchString]
  }

  proc xroe_checkPart {matchString} {
    return [xroe_matchProjectProp PART $matchString]
  }

  proc listBoards {  } {
    set counter 0;
    foreach lBoard [get_boards] {
      
      ## So the format for get_board_parts is differnt to get_boards, adjust for search
      set modBoardName [regsub ".*\.com:"  $lBoard       "" ]
      set modBoardName [regsub ":"         $modBoardName "*" ]
      set modBoardName "*${modBoardName}"
      
      ## get the board part so we can query its properties
      set board_part   [get_board_parts -filter NAME=~$modBoardName ]
      
      if { $board_part ne "" } {
        set partName [get_property PART_NAME [get_board_parts $board_part]]
        set xmlFile  [get_property FILE_NAME [get_board_parts $board_part]]              
      } else {
        set partName ""              
      }
      
      puts "[format %2d $counter] : [format %-30s $lBoard]  [format %-40s $partName] $xmlFile";
      incr counter     
      
    }
  }


  ## 
  proc breadDownWorstCasePath { pathStoreRef countKey path_from path_to } {
    upvar 1 $pathStoreRef pathStore
    set timingReport [report_timing  -from [get_pins $path_from] -to [get_pins $path_to] -return_string -no_header]]
    foreach line [split $timingReport "\n"] {
      if { [regexp {\s+Logic Levels} $line] } {
        dict set pathStore $countKey logic $line
      }
      if { [regexp {\s+Data Path Delay} $line] } {
        dict set pathStore $countKey dpath $line
      }
    } 
  }
    
  proc datetime_script {  } {
    return [ clock format [clock seconds] -format {%Y%m%d_%H%M%S} ]
  }
  
  proc breakDownWorstCasePaths { { numPaths 5 } { logicLevels 0 } {showCommands 0} { reportFile "wcPathAnalysis_DATE.csv" }} {
  
    set timingReport [report_timing  -path_type summary -max_paths $numPaths -return_string -no_header]
    set process 0
    set count 0
    set pathDict {}
    set pathStart ""
    set pathEnd ""
    foreach line [split $timingReport "\n"] {
      if  { $process } {
        if { [regexp {\s{40,}} $line] } {
          #puts "TIME: $line"
          breadDownWorstCasePath pathDict $count $pathStart $pathEnd  
          regsub {^\s*} $pathStart "" pathStart;# Update the time
          regsub {^\s*} $pathEnd   "" pathEnd;# Update the time
          dict set pathDict $count start $pathStart
          dict set pathDict $count end   $pathEnd
          dict set pathDict $count slack $line
          incr count
        } else {
          if { [regexp {\s{20,}} $line] } {
            set pathEnd $line
          } else {
            if { [regexp {^\s*[a-zA-Z]} $line] } {
              set pathStart $line
            }
          }  
        }
      }
      if { [regexp {^----} $line] } {
        set process 1
      }
    } 
  
    set stringOut [formatOutString startPoint endPoint logic 'Path%' slack]\n
    foreach report [dict keys $pathDict] {
      regexp "\/inst\/(.*)"   [dict get $pathDict $report start] allMatch startPoint
      regexp "\/inst\/(.*)"   [dict get $pathDict $report end  ] allMatch endPoint
      regexp "route(.*)"    [dict get $pathDict $report dpath] allMatch dpath
      regexp {Levels:\s+([0-9]+)}    [dict get $pathDict $report logic] allMatch logic
      regexp {\s+(.+)}    [dict get $pathDict $report slack] allMatch slack
      regsub {\s+} $slack "" slack;# Remove all white space
      if { $logic >= $logicLevels } {
        append stringOut "[formatOutString $startPoint $endPoint $logic $dpath $slack]\n"  
        if { $showCommands } {
          append stringOut "report_timing -from \[get_pins  \{[dict get $pathDict $report start]\}\] -to \[get_pins \{[dict get $pathDict $report end  ]\}\]\n"
          append stringOut "show_schematic -pin_pairs \[get_pins \{[dict get $pathDict $report start] [dict get $pathDict $report end ]\}\]\n"
        }
      }
    }
    puts $stringOut  
    regsub {DATE} $reportFile "[datetime_script]" reportFile;# Update the time
    writeStringToFile $reportFile $stringOut
  }   
  
  proc formatOutString {sp ep ll dp sl} {
    return "[format %-90s $sp],\
            [format %-90s $ep],\
            [format %7s $ll],\
            [format %-20s $dp],\
            [format %s $sl],\
             "  
  }

  proc slurpFileRetString { fileNameIn } {
    set returnString ""
    if { [ file exists  $fileNameIn ] } {
      set fp [open $fileNameIn r]
      set returnString [read $fp]
      close $fp
    } else {
      puts "Cannot open filename $fileNameIn, assuming we didn't need to."
    }
    return $returnString
  }

  package require yaml

  proc yamlFile2Dict { yamlFileName } {
    set yamlString [ slurpFileRetString $yamlFileName ]
    return [ yaml::yaml2dict $yamlString ]
  }

  proc dict2YamlFile { dictContent yamlFileName } {
    set fileId [open $yamlFileName "w"]
    puts -nonewline $fileId [ yaml::dict2yaml $dictContent ]
    close $fileId
  }

  proc writeStringToFile { fileName stringOut } {
    set fileId [open $fileName "w"]
    puts -nonewline $fileId $stringOut
    close $fileId
  }

}
