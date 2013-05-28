#!/usr/bin/perl
#  Filename:  faculty_populate.pl
#  Author:  Rick Ward <rickward@sas.upenn.edu>
#  Last Modified: May 26, 2010
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
  $min_term,);
  
$min_term = (gmtime()->year + 1900).'A';
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

initialize_fdb_faculty_temp();
populate_faculty();

$dbhMySQL->disconnect();
$dbhOracle->disconnect();

## subroutines
sub initialize_fdb_faculty_temp {
  ## print "Dropping fdb_faculty_temp\n";
  
  ## clear out any data that is there
  $dbhMySQL->do("DROP TABLE IF EXISTS fdb_faculty_temp") || die($dbhMySQL->errstr);
  
  ## print "creating fdb_faculty_temp... \n";

  $dbhMySQL->do("CREATE TABLE `fdb_faculty_temp` (
    `penn_id` int(8) NOT NULL,
    `pennkey` varchar(24) default NULL,
    `pre_name` varchar(50) default NULL,
    `first_name` varchar(100) default NULL,
    `middle_name` varchar(100) default NULL,
    `last_name` varchar(100) NOT NULL,
    `post_name` varchar(40) default NULL,
    `principal_org_code` char(4) default NULL,
    `org_descr` varchar(510) default NULL,
    `division_descr` varchar(40) default NULL,
    `primary_sas_title` varchar(2000) default NULL,
    `primary_sas_title_w_chair` varchar(2000) default NULL,
    `current_titles_w_chair` varchar(2000) default NULL,
    `rank` varchar(50) default NULL,
    `rank_sort_order` int(10) default NULL,
    `default_whse_job_class` char(6) default NULL,
    `local_add1` varchar(100) default NULL,
    `local_add2` varchar(100) default NULL,
    `mail_code` varchar(100) default NULL,
    `sas_standing_fac_start_date` date default NULL,
    `sas_standing_fac_end_date` date default NULL,
    PRIMARY KEY  (`penn_id`)
  )") || die($dbhMySQL->errstr);

  ## print "fdb_faculty_temp initialized\n";
  
}

sub populate_faculty {
  local $_;
  my ($penn_id, 
    $pennkey, 
    $pre_name, 
    $first_name, 
    $middle_name, 
    $last_name, 
    $post_name, 
    $principal_org_code, 
    $org_descr, 
    $division_descr, 
    $primary_sas_title, 
    $primary_sas_title_w_chair,
    $current_titles_w_chair,
    $rank,
    $rank_sort_order,
    $default_whse_job_class,
    $local_add1,
    $local_add2,
    $mail_code,
    $sas_standing_fac_start_date,
    $sas_standing_fac_end_date,
    $insert_faculty,
    $get_faculty,);

  ## prepare mysql insert
  $sql = "INSERT INTO fdb_faculty_temp (penn_id, pennkey, pre_name, first_name, middle_name, last_name, post_name, principal_org_code, org_descr, division_descr, primary_sas_title, primary_sas_title_w_chair, current_titles_w_chair, rank, rank_sort_order, default_whse_job_class, local_add1, local_add2, mail_code, sas_standing_fac_start_date, sas_standing_fac_end_date) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
  $insert_faculty = $dbhMySQL->prepare($sql) || die($dbhMySQL->errstr);

  ## get faculty data
  $sql = "SELECT DISTINCT penn_id, pennkey, pre_name, first_name, middle_name, last_name, post_name, principal_org_code, org_descr, division_descr, primary_sas_title, primary_sas_title_w_chair, current_titles_w_chair, rank, rank_sort_order, default_whse_job_class, local_add1, local_add2, mail_code, sas_standing_fac_start_date, sas_standing_fac_end_date FROM faculty.fdb_current_standing_faculty WHERE sas_standing_fac_end_date IS NULL";
  $get_faculty = $dbhOracle->prepare($sql) || die($dbhOracle->errstr);

  $get_faculty->execute() || die($dbhOracle->errstr);

  while (  ($penn_id, $pennkey, $pre_name, $first_name, $middle_name, $last_name, $post_name, $principal_org_code, $org_descr, $division_descr, $primary_sas_title, $primary_sas_title_w_chair, $current_titles_w_chair, $rank, $rank_sort_order, $default_whse_job_class, $local_add1, $local_add2, $mail_code, $sas_standing_fac_start_date, $sas_standing_fac_end_date) = $get_faculty->fetchrow ) {
    $insert_faculty->execute($penn_id, $pennkey, $pre_name, $first_name, $middle_name, $last_name, $post_name, $principal_org_code, $org_descr, $division_descr, $primary_sas_title, $primary_sas_title_w_chair, $current_titles_w_chair, $rank, $rank_sort_order, $default_whse_job_class, $local_add1, $local_add2, $mail_code, $sas_standing_fac_start_date, $sas_standing_fac_end_date) || die($dbhMySQL->errstr);
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
