#!/usr/bin/perl
#  Filename:  penncourses_populate.pl
#  Author:  Rick Ward <rickward@sas.upenn.edu>
#  Last Modified: August 12, 2008
#  Purpose:  This script is meant to mirror warehouse data to a local
#    MySQL table for easy access.  This script is intended to be 
#    scheduled via a regular cron job.
#
#
use DBI;
use strict;
use warnings;
use Time::gmtime;

our ($dbhOracle,
  $dbhMySQL,
  $sql,
  $get_subj_areas,
  $total_subjects,
  $total_sections,
  $min_term,
  $min_term_clean,
  $min_term_load,);
  
$min_term_clean = (gmtime()->year + 1900).'A';
$min_term_load = (gmtime()->year + 1900).'A';
$total_subjects = 0;
$total_sections = 0;

my ($subj_area);

$dbhOracle = DBI->connect("dbi:Oracle:host=pluto.sas.upenn.edu;sid=rpt","cl_user","ae2d_xxv") 
    || die "Oracle database conn failure $DBI::errstr";

$dbhMySQL = DBI->connect("dbi:mysql:database=warehousesync;host=localhost:port=3306
","whousesync_admin","swaW7W9etusW6XA3tU6T6cHu") 
    || die "MySQL database conn failure $DBI::errstr";

## the oracle "course_desc" field is XML stored in a clob field
$dbhOracle->{'LongReadLen'} = 512 * 1024;
$dbhMySQL->{'LongReadLen'} = 512 * 1024;

initialize_pc_courses_temp();
populate_subject_area_lookup();

## retrieve subject areas
$sql = "SELECT subject_area FROM sas_community.sas_subject_area_v";
$get_subj_areas = $dbhOracle->prepare($sql) || die($dbhOracle->errstr);

$get_subj_areas->execute() || die($dbhOracle->errstr);

while ( ($subj_area) = $get_subj_areas->fetchrow ) {
  populate_subject_area($subj_area);
  $total_subjects++;
}
## print "Done inserting: $total_subjects subjects and $total_sections sections.  Exiting.\n";

$dbhMySQL->disconnect();
$dbhOracle->disconnect();

## subroutines
sub initialize_pc_courses_temp {
  ## print "creating pc_courses_temp... \n";
  $dbhMySQL->do("CREATE TABLE IF NOT EXISTS `pc_courses_temp` (
    `subject_area` char(4) default NULL,
    `course_id` char(7) default NULL,
    `course_no` char(3) default NULL,
    `course_title` varchar(1422) default NULL,
    `course_instructors` varchar(1422) default NULL,
    `section_id` char(10) NOT NULL,
    `section_no` char(3) default NULL,
    `course_desc` text,
    `term` char(5) default NULL,
    `term_session` int(11) default NULL,
    `course_meeting` varchar(1422) default NULL,
    `xlists` varchar(256) default NULL,
    `activity` varchar(4) default NULL,
    `syllabus_url` varchar(100) default NULL,
    `status` char(1) default NULL,
    PRIMARY KEY  (`section_id`, `term`)
  )") || die($dbhMySQL->errstr);

  ## clear out any current (ie, from this year) data that is there
  $dbhMySQL->do("DELETE FROM pc_courses_temp WHERE term >= '$min_term_clean'") || die($dbhMySQL->errstr);
  
  ## print "pc_courses_temp initialized\n";
  
  ## now create lookup table for subject areas
  ## clear out any data that is there
  $dbhMySQL->do("DROP TABLE IF EXISTS pc_subj_area_temp") || die($dbhMySQL->errstr);

  $dbhMySQL->do("CREATE TABLE `pc_subj_area_temp` (
    `subject_area` char(4) NOT NULL,
    `subject_area_desc` varchar(40) NOT NULL,
    PRIMARY KEY  (`subject_area`)
  )") || die($dbhMySQL->errstr);
}

sub populate_subject_area {
  local $_;
  my $subj_area = shift;
  my ($subject_area, 
    $course_id, 
    $course_no, 
    $course_title, 
    $course_instructors, 
    $section_id, 
    $section_no, 
    $course_desc, 
    $term, 
    $term_session, 
    $course_meeting, 
    $xlists,
    $activity,
    $syllabus_url,
    $status,
    $get_courses,
    $insert_section,
    $update_section,);

  ## print "Selecting $subj_area courses...\n";

  ## prepare mysql insert
  $sql = "INSERT INTO pc_courses_temp (subject_area, course_id, course_no, course_title, course_instructors, section_id, section_no, course_desc, term, term_session, course_meeting, xlists, activity, syllabus_url, status) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
  $insert_section = $dbhMySQL->prepare($sql) || die($dbhMySQL->errstr);

  ## prepare mysql insert
  $sql = "UPDATE pc_courses_temp SET subject_area = ?, course_id = ?, course_no = ?, course_title = ?, course_instructors = ?, section_no = ?, course_desc = ?, course_meeting = ?, xlists = ?, activity = ?, syllabus_url = ?, status = ? WHERE section_id = ? AND term = ? AND term_session = ?";
  $update_section = $dbhMySQL->prepare($sql) || die($dbhMySQL->errstr);

  ## loop over extracted courses
  $sql = "SELECT DISTINCT subject_area, course_id, course_no, course_title, course_instructors, section_id, section_no, course_desc, term, term_session, course_meeting, xlists, activity, syllabus_url, status FROM sas_community.drupalcourselist_v WHERE subject_area = ? AND course_id IS NOT NULL AND term >='$min_term_load'";
  $get_courses = $dbhOracle->prepare($sql) || die($dbhOracle->errstr);

  $get_courses->execute($subj_area) || die($dbhOracle->errstr);

  while (  ($subject_area, $course_id, $course_no, $course_title, $course_instructors, $section_id, $section_no, $course_desc, $term, $term_session, $course_meeting, $xlists, $activity, $syllabus_url, $status) = $get_courses->fetchrow ) {
    ## $insert_section->execute(trim($subject_area), trim($course_id), trim($course_no), trim($course_title), trim($course_instructors), trim($section_id), trim($section_no), trim($course_desc), trim($term), trim($term_session), trim($course_meeting), trim($xlists), trim($activity), trim($syllabus_url), trim($status)) || die($dbhMySQL->errstr);
    eval {
        $insert_section->execute(trim($subject_area), trim($course_id), trim($course_no), trim($course_title), trim($course_instructors), trim($section_id), trim($section_no), trim($course_desc), trim($term), trim($term_session), trim($course_meeting), trim($xlists), trim($activity), trim($syllabus_url), trim($status)) || die($dbhMySQL->errstr);
        ## print "inserted $section_id $term\n";
    };
    if ($@) {
        $update_section->execute(trim($subject_area), trim($course_id), trim($course_no), trim($course_title), trim($course_instructors), trim($section_no), trim($course_desc), trim($course_meeting), trim($xlists), trim($activity), trim($syllabus_url), trim($status), trim($section_id), trim($term), trim($term_session)) || die($dbhMySQL->errstr);
        ## print "updated $section_id $term\n";
    }
    $total_sections++;
  }
  ## print "Done inserting $subj_area. Exiting subroutine populate_subject_area.\n";
}

sub populate_subject_area_lookup {
  my ($subject_area, 
    $subject_area_desc, 
    $insert_subj_area,
    $get_subject_areas,);
  ## prepare mysql insert
  $sql = "INSERT INTO pc_subj_area_temp (subject_area, subject_area_desc) VALUES (?, ?)";
  $insert_subj_area = $dbhMySQL->prepare($sql) || die($dbhMySQL->errstr);
	
  ## retrieve subject areas from Oracle
  $sql = "SELECT DISTINCT subject_area, subject_area_desc FROM sas_community.sas_subject_area_v ORDER BY subject_area";
  $get_subject_areas = $dbhOracle->prepare($sql) || die($dbhOracle->errstr);

  $get_subject_areas->execute() || die($dbhOracle->errstr);
  
  while (  ($subject_area, $subject_area_desc) = $get_subject_areas->fetchrow ) {
    $insert_subj_area->execute($subject_area, trim($subject_area_desc)) || die($dbhMySQL->errstr);
  }
}

# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
    my $string = shift;
    if ($string) {
	    $string =~ s/^\s+//;
	    $string =~ s/\s+$//;
	    return $string;
    }
    return $string;
}
