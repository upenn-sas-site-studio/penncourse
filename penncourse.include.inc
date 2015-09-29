<?php
// utility functions for the Penncourse module

/**
 * function penncourse_build_course_node
 *
 * Create or update course nodes (pc_course) based on data passed in $values_array
 * @param integer $nid
 * @param array $values_array
 */
function penncourse_build_course_node($nid = NULL, $values_array)
{
    global $user;
    $current_uid = $user->uid;
    // we want the author of this content to be a dedicated user account, sas_event_calendar
    $user = user_load(array('name' => 'penncourse_user'));
    module_load_include('inc', 'node', 'node.pages');

    // pcpm($values_array);

    if ($nid) {
        $action = 'updating course node: ' . $nid . '[nid] ' . $values_array['course_id'] . '[course_id]';
        $node = node_load($nid, NULL, TRUE);
    } else {
        $action = 'new course node: ' . $values_array['course_id'] . '[course_id]';
        $node = new stdClass;
        $node->type = 'pc_course';
        node_object_prepare($node);
        $node->uid = $user->uid;
        $node->name = 'penncourse_user';
        $node->created = REQUEST_TIME;
        $node->status = 1;
    }

    $node->title = $values_array['course_id'] . ' - ' . $values_array['title'];
    $node->field_pc_descr[0]['value'] = $values_array['descr'];
    $node->field_pc_syllabus[0]['url'] = $values_array['syllabus_url'];
    $node->field_pc_course_id[0]['value'] = $values_array['course_id'];
    $node->field_pc_title[0]['value'] = $values_array['title'];
    $node->field_pc_term[0]['value'] = $values_array['term'];
    $node->field_pc_term_session[0]['value'] = $values_array['term_session'];
    $node->field_pc_subj_area[0]['value'] = $values_array['subj_area'];
    $node->field_pc_dist_req[0]['value'] = $values_array['dist_req'];
    $node->field_pc_status[0]['value'] = $values_array['status'];

    node_save($node);
    // pcpm($node);
    watchdog('penncourse cron', $action . '<br />' . l($node->title, 'node/' . $node->nid), array(), WATCHDOG_NOTICE);
    $nid = $node->nid;
    unset($node);

    // reload our original user
    $user = user_load($current_uid);

    return $nid;
} // function penncourse_build_course_node

/**
 * function penncourse_build_section_node
 *
 * Create or update section nodes (pc_section) based on data passed in $values_array
 * @param integer $nid
 * @param array $values_array
 */
function penncourse_build_section_node($nid = NULL, $values_array)
{
    global $user;
    $current_uid = $user->uid;
    // we want the author of this content to be a dedicated user account, sas_event_calendar
    $user = user_load(array('name' => 'penncourse_user'));
    module_load_include('inc', 'node', 'node.pages');

    // pcpm($values_array);

    if ($nid) {
        $action = 'updating section node: ' . $nid . '[nid] ' . $values_array['section_id'] . '[section_id]';
        $node = node_load($nid, NULL, TRUE);
    } else {
        $action = 'new section node: ' . $values_array['section_id'] . '[section_id]';
        $node = new stdClass;
        $node->type = 'pc_section';
        node_object_prepare($node);
        $node->uid = $user->uid;
        $node->name = 'penncourse_user';
        $node->created = REQUEST_TIME;
        $node->status = 1;
    }

    $node->title = $values_array['course_id'] . ' - ' . $values_array['title'];
    // $node->field_pc_descr[0]['value'] = $values_array['descr']; this field is maintained locally by departments
    $node->field_pc_instructors[0]['value'] = $values_array['instructors'];
    if ($values_array['status'] == 'X') {
        $node->field_pc_meeting[0]['value'] = 'CANCELED';
    } else {
        $node->field_pc_meeting[0]['value'] = $values_array['meeting'];
    }
    $node->field_pc_title[0]['value'] = $values_array['title'];
    $node->field_pc_location[0]['value'] = $values_array['location'];
    $node->field_pc_section_id[0]['value'] = $values_array['section_id'];
    $node->field_pc_course[0]['nid'] = $values_array['course'];
    $node->field_pc_subj_area[0]['value'] = $values_array['subj_area'];
    $node->field_pc_term[0]['value'] = $values_array['term'];
    $node->field_pc_term_session[0]['value'] = $values_array['term_session'];
    $node->field_pc_activity[0]['value'] = $values_array['activity'];
    $node->field_pc_course_no[0]['value'] = $values_array['course_no'];
    $node->field_pc_section_no[0]['value'] = $values_array['section_no'];
    $node->field_pc_xlist[0]['value'] = $values_array['xlist'];
    $node->field_pc_syllabus_url[0]['url'] = $values_array['syllabus_url'];
    $node->field_pc_status[0]['value'] = $values_array['status'];
    $node->field_pc_course_id[0]['value'] = $values_array['course_id'];
    $node->field_pc_sec_reg_ctrl[0]['value'] = $values_array['sec_reg_ctrl'];
    $node->field_pc_sec_type[0]['value'] = $values_array['sec_type'];

    node_save($node);
    watchdog('penncourse cron', $action . '<br />' . l($node->title, 'node/' . $node->nid), array(), WATCHDOG_NOTICE);
    $nid = $node->nid;
    unset($node);

    // reload our original user
    $user = user_load($current_uid);

    return $nid;
} // function penncourse_build_section_node

/**
 * function penncourse_dynamic_terms()
 *
 * Copied over from LPSCPS module, not currently used
 * Creates a text list for populating a drop down of terms.
 * Accepts number of TERMS both forward (from current year) and back.
 */
function penncourse_dynamic_terms($number_forward, $number_backward)
{

    $year = intval(date(Y));
    $year_min = intval($year - ($number_backward / 3));
    $year_max = intval($year + ($number_forward / 3));
    $year_increment = $year_min;
    $term_codes = array('A', 'B', 'C');
    $seasons = array(' Spring', ' Summer', ' Fall');
    $terms = array();

    while ($year_increment <= $year_max) { //loop through years from min to max
        for ($i = 0; $i < 3; $i++) { //increment season
            $terms[$year_increment . $term_codes[$i]] = $year_increment . $seasons[$i];
        }
        $year_increment++;
    }
    //create drupal option list
    $term_options = array();
    foreach ($terms as $key => $value) {
        $term_options[$key] = $value;
    }

    return $term_options;
} // function penncourse_dynamic_terms

/**
 * function penncourse_session_descriptions
 *
 * Copied over from LPSCPS module, not currently used
 * return the session descriptions used for the various Summer Session codes
 */
function penncourse_session_descriptions()
{
    return array('' => "12-Week Session",
        '1' => "6-Week Summer Session I",
        '2' => "6-Week Summer Session II",
        'S' => "Special Session");
} // function penncourse_session_descriptions

/**
 * function _penncourse_load_keyed_nids($type_name, $field_name)
 *
 * Copied over from LPSCPS module, not currently used
 * returns an array of nids (node id's) keyed by the field value specified by $field_name
 * may only be useful in the case of simple content types with a single identifying key field
 */
function _penncourse_load_keyed_nids($type_name, $field_name)
{
    // get database info about content fields
    $db_info = penncourse_db_info($type_name, array($field_name));

    $nids = array();
    // get nids and the key specified by $field_name
    $results = db_query("SELECT DISTINCT(n.nid) AS nid,
											c.%s AS %s
											FROM {node} n 
 											LEFT JOIN {%s} c ON n.vid = c.vid
 											WHERE n.type in ('%s')
											GROUP BY nid
											ORDER BY %s ASC",
        $db_info[$field_name]['column'],
        $field_name,
        $db_info[$field_name]['table'],
        $type_name,
        $field_name);


    while ($key_array = db_fetch_object($results)) {
        $nids[$key_array->$field_name] = $key_array->nid;
    }
    unset($results);

    return $nids;

} // function _penncourse_load_keyed_nids

/**
 * function penncourse_load_course_nids
 *
 * returns an array of nids (node id's) keyed by the CPS course_key value for that course record
 * lookup is restricted to a single term
 * @param integer $year
 * @param string $subj_area
 */
function penncourse_load_course_nids($term, $subj_area = NULL)
{
    if ($term) {
        // get database info about course fields
        $db_info = penncourse_db_info('pc_course', array('field_pc_course_id', 'field_pc_term', 'field_pc_subj_area'));

        $nids = array();
        // get nids and the course_key
        $sql = sprintf("			SELECT 		n.nid AS nid,
   																	c.%s AS field_pc_course_id,
   																	t.%s AS field_pc_term
 												 	FROM 			{node} n 
 												 	LEFT JOIN {%s} t ON n.vid = t.vid
 												 	LEFT JOIN {%s} c ON n.vid = c.vid
 												 	LEFT JOIN {%s} s ON n.vid = s.vid
 												 	WHERE 		(n.type in ('pc_course')) AND (t.%s = '%s') AND (s.%s = '%s')
   											 	ORDER BY 	field_pc_course_id ASC",
            $db_info['field_pc_course_id']['column'],
            $db_info['field_pc_term']['column'],
            $db_info['field_pc_term']['table'],
            $db_info['field_pc_course_id']['table'],
            $db_info['field_pc_subj_area']['table'],
            $db_info['field_pc_term']['column'],
            $term,
            $db_info['field_pc_subj_area']['column'],
            $subj_area);
        $results = db_query("	SELECT 		n.nid AS nid,
   																	c.%s AS field_pc_course_id,
   																	t.%s AS field_pc_term
 												 	FROM 			{node} n 
 												 	LEFT JOIN {%s} t ON n.vid = t.vid
 												 	LEFT JOIN {%s} c ON n.vid = c.vid
 												 	LEFT JOIN {%s} s ON n.vid = s.vid
 												 	WHERE 		(n.type in ('pc_course')) AND (t.%s = '%s') AND (s.%s = '%s')
   											 	ORDER BY 	field_pc_course_id ASC",
            $db_info['field_pc_course_id']['column'],
            $db_info['field_pc_term']['column'],
            $db_info['field_pc_term']['table'],
            $db_info['field_pc_course_id']['table'],
            $db_info['field_pc_subj_area']['table'],
            $db_info['field_pc_term']['column'],
            $term,
            $db_info['field_pc_subj_area']['column'],
            $subj_area);

        pcpm('course node lookup SQL: ' . $sql);

        while ($key_array = db_fetch_object($results)) {
            $nids[trim($key_array->field_pc_term) . '-' . trim($key_array->field_pc_course_id)] = $key_array->nid;
        }
        unset($results);

        return $nids;

    } else {
        return NULL;
    }
} // function penncourse_load_course_nids

/**
 * function penncourse_load_section_nids
 *
 * returns an array of nids (node id's) keyed by the CPS course_key value for that course record
 * lookup is restricted to a single term
 * @param integer $year
 * @param string $subj_area
 */
function penncourse_load_section_nids($term, $subj_area = NULL)
{
    if ($term) {
        // get database info about course fields
        $db_info = penncourse_db_info('pc_section', array('field_pc_section_id', 'field_pc_term', 'field_pc_subj_area'));

        $nids = array();
        // get nids and the course_key
        $sql = sprintf("			SELECT 		n.nid AS nid,
   																	c.%s AS field_pc_section_id,
   																	t.%s AS field_pc_term
 												 	FROM 			{node} n 
 												 	LEFT JOIN {%s} t ON n.vid = t.vid
 												 	LEFT JOIN {%s} c ON n.vid = c.vid
 												 	LEFT JOIN {%s} s ON n.vid = s.vid
 												 	WHERE 		(n.type in ('pc_section')) AND (t.%s = '%s') AND (s.%s = '%s')
   											 	ORDER BY 	field_pc_section_id ASC",
            $db_info['field_pc_section_id']['column'],
            $db_info['field_pc_term']['column'],
            $db_info['field_pc_term']['table'],
            $db_info['field_pc_section_id']['table'],
            $db_info['field_pc_subj_area']['table'],
            $db_info['field_pc_term']['column'],
            $term,
            $db_info['field_pc_subj_area']['column'],
            $subj_area);
        $results = db_query("	SELECT 		n.nid AS nid,
   																	c.%s AS field_pc_section_id,
   																	t.%s AS field_pc_term
 												 	FROM 			{node} n 
 												 	LEFT JOIN {%s} t ON n.vid = t.vid
 												 	LEFT JOIN {%s} c ON n.vid = c.vid
 												 	LEFT JOIN {%s} s ON n.vid = s.vid
 												 	WHERE 		(n.type in ('pc_section')) AND (t.%s = '%s') AND (s.%s = '%s')
   											 	ORDER BY 	field_pc_section_id ASC",
            $db_info['field_pc_section_id']['column'],
            $db_info['field_pc_term']['column'],
            $db_info['field_pc_term']['table'],
            $db_info['field_pc_section_id']['table'],
            $db_info['field_pc_subj_area']['table'],
            $db_info['field_pc_term']['column'],
            $term,
            $db_info['field_pc_subj_area']['column'],
            $subj_area);

        pcpm('section node lookup SQL: ' . $sql);

        while ($key_array = db_fetch_object($results)) {
            $nids[trim($key_array->field_pc_term) . '-' . trim($key_array->field_pc_section_id)] = $key_array->nid;
        }
        unset($results);

        return $nids;

    } else {
        return NULL;
    }
} // function penncourse_load_section_nids

/**
 * function penncourse_process_subj_area($subj_area,$year)
 * processes the course and section entries for a single subject area
 */
function penncourse_process_subj_area($subj_area, $year = NULL)
{
    if (!$year || ($year == date('Y', time()))) {
        $start_term = penncourse_start_term();
        $end_term = penncourse_load_final_term();
    } else {
        $start_term = $year . 'A';
        $end_term = $year . 'C';
    }
    $prev_course_term = '';
    $course_nid = null;

    // build an array of terms to process
    $terms = array();
    $year_increment = substr($start_term, 0, 4);
    $year_max = substr($end_term, 0, 4);
    $term_codes = array('A', 'B', 'C');

    while ($year_increment <= $year_max) { //loop through years from min to max
        for ($i = 0; $i < 3; $i++) { //increment season
            $terms[] = $year_increment . $term_codes[$i];
        }
        $year_increment++;
    }

    foreach ($terms as $term) {
        if (($term >= $start_term) && ($term <= $end_term)) {
            penncourse_load_courses_by_subj($term, $subj_area);
        }
    }

    watchdog('penncourse cron', 'end term: ' . $end_term, array(), WATCHDOG_NOTICE);

} // function penncourse_process_subj_area()

/**
 * function penncourse_load_courses_by_subj
 *
 * Load course data from warehousesync.pc_courses_temp and build course
 * and section nodes for a given term and subject area
 * @param string $term
 * @param string $subj_area
 */
function penncourse_load_courses_by_subj($term, $subj_area)
{
    pcpm(array('term' => $term, 'subj_area' => $subj_area));

    // only run if term has been set in the admin interface
    if ($term) {
        // get course node id's
        $course_nodes = penncourse_load_course_nids($term, $subj_area);
        pcpm('course nid lookup');
        pcpm($course_nodes);
        $section_nodes = penncourse_load_section_nids($term, $subj_area);
        pcpm('section nid lookup');
        pcpm($section_nodes);

        // get course data
        $sql = sprintf("SELECT 	p.subject_area,
    															p.course_id, 
    															p.course_no, 
    															p.course_inventory_title,
    															p.course_title,
    															p.course_instructors,
    															p.section_id, 
    															p.section_no, 
    															p.course_desc, 
    															p.term, 
    															p.term_session, 
    															p.course_meeting, 
    															p.xlists, 
    															p.activity, 
    															p.syllabus_url, 
    															p.distribution_requirement,
    															p.section_reg_control,
    															p.section_type_term,
    															p.status
                        FROM 			{warehousesync.pc_courses_temp} p
												WHERE 		p.term = '%s' 
                        AND 			p.subject_area = '%s'
                        ORDER BY 	p.course_no, p.section_no",
            $term,
            $subj_area);
        $results = db_query("SELECT 	p.subject_area,
    															p.course_id, 
    															p.course_no, 
    															p.course_inventory_title,
    															p.course_title,
    															p.course_instructors, 
    															p.section_id, 
    															p.section_no, 
    															p.course_desc, 
    															p.term, 
    															p.term_session, 
    															p.course_meeting, 
    															p.xlists, 
    															p.activity, 
    															p.syllabus_url, 
    															p.distribution_requirement, 
    															p.section_reg_control,
    															p.section_type_term,
    															p.status
                        FROM 			warehousesync.pc_courses_temp p
												WHERE 		p.term = '%s' 
                        AND 			p.subject_area = '%s'
                        ORDER BY 	p.course_no, p.section_no",
            $term,
            $subj_area);
        pcpm($sql);

        // build course and section arrays, keyed to term-course_id and term_section_id
        $courses = array();
        $sections = array();
        $course_id = '';
        while ($course_record = db_fetch_object($results)) {
            if ($course_record->course_id != $course_id) {
                $courses[trim($course_record->term) . '-' . trim($course_record->course_id)] = $course_record;
                $course_id = $course_record->course_id;
            }
            $sections[trim($course_record->term) . '-' . trim($course_record->section_id)] = $course_record;
        }
        unset($results);

        pcpm('course data');
        pcpm($courses);
        pcpm('section data');
        pcpm($sections);

        // loop through $courses array and update or create pc_course nodes as needed
        foreach ($courses as $key => $course_record) {
            $course_info = array();
            $course_info['title'] = $course_record->course_inventory_title;
            $course_info['descr'] = penncourse_transform_description($course_record->course_desc);
            $course_info['syllabus_url'] = $course_record->syllabus_url;
            $course_info['course_id'] = $course_record->course_id;
            $course_info['term'] = $course_record->term;
            $course_info['term_session'] = $course_record->term_session;
            $course_info['subj_area'] = $course_record->subject_area;
            $course_info['dist_req'] = penncourse_translate_dist_req($course_record->distribution_requirement);
            $course_info['status'] = $course_record->status;

            if (isset($course_nodes[$key])) {
                penncourse_build_course_node($course_nodes[$key], $course_info);
            } else {
                $course_nodes[$key] = penncourse_build_course_node(NULL, $course_info);
            }
        }

        // loop through $sections array and update or create pc_section nodes as needed
        $current_term = penncourse_current_term();
        foreach ($sections as $key => $section_record) {
            $section_info = array();
            $section_info['title'] = $section_record->course_title;
            // $section_info['descr'] = penncourse_transform_description($section_record->course_desc); this field is maintained locally on the Drupal site
            $section_info['instructors'] = penncourse_transform_instructors($section_record->course_instructors);
            // only publish meeting information for the current term or earlier
            $section_info['meeting'] = penncourse_transform_meetings($section_record->course_meeting);
            if ($section_record->term <= $current_term) {
                $section_info['location'] = penncourse_transform_location($section_record->course_meeting);
            }
            else {
                $section_info['location'] = '';
            }
            $section_info['section_id'] = $section_record->section_id;
            $section_info['course'] = $course_nodes[trim($section_record->field_pc_term) . '-' . trim($section_record->field_pc_course_id)];
            $section_info['subj_area'] = $section_record->subject_area;
            $section_info['term'] = $section_record->term;
            $section_info['term_session'] = $section_record->term_session;
            $section_info['activity'] = $section_record->activity;
            $section_info['course_no'] = $section_record->course_no;
            $section_info['section_no'] = $section_record->section_no;
            $section_info['xlist'] = $section_record->xlists;
            $section_info['syllabus_url'] = $section_record->syllabus_url;
            $section_info['sec_reg_ctrl'] = penncourse_transform_sec_reg_ctrl($section_record->section_reg_control);
            $section_info['sec_type'] = penncourse_transform_sec_type($section_record->section_type_term);
            $section_info['status'] = $section_record->status;
            $section_info['course_id'] = $section_record->course_id;

            if (isset($section_nodes[$key])) {
                penncourse_build_section_node($section_nodes[$key], $section_info);
            } else {
                $section_nodes[$key] = penncourse_build_section_node(NULL, $section_info);
            }
        }

        global $user;
        $current_uid = $user->uid;
        // we want the author of this content to be a dedicated user account, sas_event_calendar
        $user = user_load(array('name' => 'penncourse_user'));

        // if a course record no longer exists for the corresponding pc_course node, delete the node
        foreach ($course_nodes as $key => $nid) {
            if (!isset($courses[$key])) {
                node_delete($nid);
            }
        }

        // if a section record no longer exists for the corresponding pc_section node, delete the node
        foreach ($section_nodes as $key => $nid) {
            if (!isset($sections[$key])) {
                node_delete($nid);
            }
        }
        // reload our original user
        $user = user_load($current_uid);
    }
} // function penncourse_load_courses_by_subj

/**
 * function penncourse_is_summer_term
 *
 * Copied over from LPSCPS module, not currently used
 * Returns boolean
 */
function penncourse_is_summer_term()
{
    return (substr(variable_get('penncourse_cycle_term', ''), 4, 1) == 'B');
}

/**
 * function penncourse_xml_to_array
 *
 * Copied over from LPSCPS module, not currently used
 * given a simple 3-element-deep XML structure (<root><record><param1 /><param2 /></record></root>), returns a keyed array
 * @param xml $xmlstring
 * @return array
 */
function penncourse_xml_to_array($xmlstring)
{
    $result = array();
    try {
        $xml = new SimpleXMLElement($xmlstring);
        foreach ($xml->xpath('/*/*') as $record) {
            $record_array = array();
            foreach ($record->children() as $element) {
                $record_array[$element->getName()] = (string)$element;
            }
            $result[] = $record_array;
        }
    }
    catch (Exception $e) {
        penncourse_log_exception($e, $xmlstring);
    }

    return $result;
} // function penncourse_xml_to_array

/**
 * function penncourse_simple_xml_to_array
 *
 * Copied over from LPSCPS module, not currently used
 * given a VERY simple 2-element-deep XML structure (<root><record1 /><record2 />... </root>), returns an array
 * @param xml $xmlstring
 * @return array
 */
function penncourse_simple_xml_to_array($xmlstring)
{
    $result = array();
    try{
        $xml = new SimpleXMLElement($xmlstring);
        foreach ($xml->xpath('/*/*') as $record) {
            $result[] = (string)$record;
        }
    }
    catch (Exception $e) {
        penncourse_log_exception($e, $xmlstring);
    }

    return $result;
} // function penncourse_simple_xml_to_array

function penncourse_term_is_valid($term)
{
    if (preg_match('/^[0-9]{4}[A-C]$/', $term)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * function penncourse_translate_dist_req
 *
 * return a formatted distribution requirement string based on a term code
 * @param string $term
 */
function penncourse_translate_dist_req($req_code)
{
    $req_array = array(
        'A' => 'Arts & Letters Sector (all classes)',
        'B' => 'Hum/Soc Sci or Nat Sci/Math (new curriculum only)',
        'C' => 'General Requirement in Science Studies',
        'F' => 'General Requirement in Formal Reasoning & Analysis',
        'H' => 'History & Tradition Sector (all classes)',
        'L' => 'Living World Sector (all classes)',
        'O' => 'Hum & Soc Sci Sector (new curriculum only)',
        'N' => 'Nat Sci & Math Sector (new curriculum only)',
        'P' => 'Physical World Sector (all classes)',
        'R' => 'Writing Requirement Course',
        'S' => 'Society sector (all classes)',
        'T' => 'General Requirement in Society',
        'U' => 'General Requirement in History & Tradition',
        'V' => 'General Requirement in Arts & Letters',
        'W' => 'General Requirement in Formal Reasoning & Analysis',
        'X' => 'General Requirement in Living World',
        'Y' => 'General Requirement in Physical World',
        'Z' => 'General Requirement in Science Studies',
        '1' => 'Distributional course in Society (class of 09 and prior)',
        '2' => 'Distributional course in History & Tradition (class of 09 and prior)',
        '3' => 'Distributional course in Arts & Letters (class of 09 and prior)',
    );

    if (isset($req_array[$req_code])) {
        return $req_array[$req_code];
    } else {
        return NULL;
    }
} // function penncourse_translate_dist_req

/**
 * parse $sec_reg_ctrl XML and translate control codes into text
 *
 * @param string $sec_reg_ctrl
 */
function penncourse_transform_sec_reg_ctrl($sec_reg_ctrl) {
    $result_array = array();

    if ($sec_reg_ctrl) {
        $sec_reg_ctrl_array = penncourse_xml_to_array($sec_reg_ctrl);

        foreach ($sec_reg_ctrl_array as $record) {
            if ((substr($record['reg-control-desc'], 0, 5) != 'QUOTA') && ($record['reg-control-desc'] != 'CONTINUATION OF ABOVE QUOTA')) {
                $result_array[] = $record['reg-control-desc'];
            }
        }
    }

    $result = implode('; ', $result_array);
    return $result;
}

/**
 * parse $sec_type XML and translate control codes into text
 *
 * @param string $sec_type
 */
function penncourse_transform_sec_type($sec_type) {
    $result_array = array();

    if ($sec_type) {
        $sec_type_array = penncourse_xml_to_array($sec_type);

        foreach ($sec_type_array as $record) {
            $result_array[] = $record['section-type-desc'];
        }
    }

    $result = implode('; ', $result_array);
    return $result;
}

/**
 * function penncourse_translate_day
 *
 * Copied over from LPSCPS module, not currently used
 * Returns a day name for a given day code
 * @param string $d day code
 */
function penncourse_translate_day($d)
{
    $days = array(
        'M' => 'Monday',
        'T' => 'Tuesday',
        'W' => 'Wednesday',
        'R' => 'Thursday',
        'F' => 'Friday',
        'S' => 'Saturday',
        'U' => 'Sunday',
    );

    if (isset($days[$d])) {
        return $days[$d];
    } else {
        return NULL;
    }
} // function penncourse_translate_day

/**
 * function penncourse_transform_description
 * input xml string
 * output text
 */
function penncourse_transform_description($xmlstring)
{
    $output = '';
    try {
        $xml = new SimpleXMLElement($xmlstring);
        foreach ($xml->xpath('/div[p]') as $description) {
            // $output .= '<ul class="penncourse-course-location">';
            foreach ($description->p as $para) {
                $output .= $para;
                $output .= chr(10) . chr(13) . chr(10) . chr(13);
            }
        }
    }
    catch (Exception $e) {
        penncourse_log_exception($e, $xmlstring);
    }

    return $output;
} // function penncourse_transform_description

/**
 * function penncourse_transform_instructors
 * input xml string
 * output xhtml formatted text
 */
function penncourse_transform_instructors($xmlstring)
{
    $xhtml = '';
    try {
        $xml = new SimpleXMLElement($xmlstring);
        foreach ($xml->xpath("/instructors[instructor]") as $instructors) {
            // $xhtml .= '<ul class="penncourse-course-instructors">';
            foreach ($instructors->instructor as $instructor) {
                if ($instructor->lastname) {
                    $xhtml .= '<span class="penncourse-course-instructor" id="' . $instructor->pennkey . '">' . $instructor->lastname . ', ' . $instructor->firstname . '</span><br />';
                }
            }
            // $xhtml .= '</ul>';
        }
    }
    catch (Exception $e) {
        penncourse_log_exception($e, $xmlstring);
    }

    return $xhtml;
} // function penncourse_transform_instructors()

/**
 * function penncourse_transform_meetings
 * input xml string
 * output xhtml formatted text
 */
function penncourse_transform_meetings($xmlstring)
{
    $xhtml = '';
    try {
        $xml = new SimpleXMLElement($xmlstring);
        foreach ($xml->xpath('/meetings[meeting]') as $meetings) {
            foreach ($meetings->meeting as $meeting) {
                if ($meeting->days && $meeting->time) {
                    $xhtml .= '<span class="penncourse-course-meeting">' . $meeting->days . ' ' . $meeting->time . '</span><br />';
                }
            }
        }
    }
    catch (Exception $e) {
        penncourse_log_exception($e, $xmlstring);
    }

    return $xhtml;
} // function penncourse_transform_meetings

/**
 * function penncourse_transform_location
 * input xml string
 * output xhtml formatted text
 */
function penncourse_transform_location($xmlstring)
{
    $xhtml = '';
    try {
        $xml = new SimpleXMLElement($xmlstring);
        foreach ($xml->xpath('/meetings[meeting]') as $meetings) {
            // $xhtml .= '<ul class="penncourse-course-location">';
            foreach ($meetings->meeting as $meeting) {
                // $xhtml .= '<li><span style="font-weight:bold">Room: </span>'.$meeting->room.' &nbsp; '.$meeting->building.'</li>';
                if ($meeting->building && $meeting->room) {
                    $xhtml .= '<span class="penncourse-course-location">' . $meeting->building . ' ' . $meeting->room . '</span><br />';
                }
            }
            // $xhtml .= '</ul>';
        }
    }
    catch (Exception $e) {
        penncourse_log_exception($e, $xmlstring);
    }

    // echo $xhtml.chr(10).chr(13);
    return $xhtml;
} // function penncourse_transform_location

/**
 * function penncourse_db_info
 *
 * Given a content type and array of field names in that content type,
 * returns a keyed array of the database table and column names
 *
 * Only valid for simple field types (textfield, integer)
 *
 * $content_type     string The machine name of the content type
 * $field_name_array array  An array of field names
 */
function penncourse_db_info($content_type, $field_name_array)
{
    $result = array();

    foreach ($field_name_array as $field_name) {
        $db_info = content_database_info(content_fields($field_name, $content_type));
        $result[$field_name]['table'] = $db_info['table'];
        $result[$field_name]['column'] = $db_info['columns']['value']['column'];
    }
    return $result;
} // function penncourse_db_info

/**
 * function penncourse_start_term()
 *
 * returns the term code for the first semester of the current year
 * this helps penncourse_cleanup() to only process current records
 * (ie, we don't want to waste time cleaning up old data that is not of current use)
 */
function penncourse_start_term()
{
    $term = date('Y', time());
    $term .= 'A';
    return $term;
} // function penncourse_start_term()

/**
 * function penncourse_load_final_term()
 * returns the term code for the most currently published course roster
 *
 * course rosters for the summer and fall are finalized on 2/19
 * rosters for the spring are finalized on 10/1
 *
 */
function penncourse_load_final_term()
{
    if ((date('n', time()) >= 10) && (date('j', time()) >= 1)) {
        // date is after 10/1 - final term is next spring term
        $term = date('Y', time()) + 1;
        $term .= 'A';
    } elseif ((date('n', time()) >= 2) && ((date('j', time()) >= 19) || (date('n', time()) >= 3))) {
        // date is after 2/19 - final term is next (or current) fall term
        $term = date('Y', time());
        $term .= 'C';
    } else {
        // date is before 2/19 - final term is current spring term
        $term = date('Y', time());
        $term .= 'A';
    }
    return $term;
} // function penncourse_load_final_term()

/**
 * function penncourse_translate_term($term_code)
 * returns a formatted string of the term name for a given $term_code
 * ('2008C' returns 'Fall 2008')
 */
function penncourse_translate_term($term_code)
{
    $term_name = array('A' => 'Spring', 'B' => 'Summer', 'C' => 'Fall');
    if (is_numeric(substr($term_code, 0, 4)) && ((strtoupper(substr($term_code, 4, 1)) == 'A') || (strtoupper(substr($term_code, 4, 1)) == 'B') || (strtoupper(substr($term_code, 4, 1)) == 'C'))) {
        return $term_name[strtoupper(substr($term_code, 4, 1))] . ' ' . substr($term_code, 0, 4);
    } else {
        // invalid code
        return 'Invalid term code';
    }
} // function penncourse_translate_term()

/**
 * function penncourse_translate_subject($subj_code)
 *
 * returns a formatted string of Subject Area name
 * ('ANTH' returns 'Anthropology')
 */
function penncourse_translate_subject($subj_code)
{
    $subj_descr = '';
    if (preg_match('/[A-Za-z][A-Za-z][A-Za-z][A-Za-z]?/', $subj_code, $matches)) {
        $subj_code = strtoupper($matches[0]);
        $sql = sprintf("SELECT DISTINCT subject_area_desc " .
            "FROM warehousesync.pc_subj_area_temp " .
            "WHERE subject_area = '%s'",
            $subj_code);

        $result = db_query($sql);
        $subj_descr = db_result($result);
        unset($result);
    }
    return $subj_descr;
} // function penncourse_translate_subject()

/**
 * function penncourse_current_term()
 * returns the current term
 * A = January 1 through May 20
 * B = May 21 through August 15
 * C = August 16 through December 31
 */
function penncourse_current_term()
{
    // get database information about course_course_id field
    $term = date('Y', time());
    if ((date('n', time()) <= 5) && ((date('j', time()) <= 20) || (date('n', time()) <= 4))) {
        $term .= 'A';
    } elseif ((date('n', time()) <= 8) && ((date('j', time()) <= 15) || (date('n', time()) <= 7))) {
        $term .= 'B';
    } else {
        $term .= 'C';
    }
    return $term;
} // function penncourse_current_term()

/**
 * penncourse_load_course_node($course_id)
 *
 * load course node for a course id and term
 * returns node object
 */
function penncourse_load_course_node($course_id, $term)
{
    $course_table = array();
    $course_field = array();
    $db_info = content_database_info(content_fields('field_pc_course_id', 'pc_course'));
    $course_table['course_id'] = $db_info['table'];
    $course_field['course_id'] = $db_info['columns']['value']['column'];
    $db_info = content_database_info(content_fields('field_pc_term', 'pc_course'));
    $course_table['term'] = $db_info['table'];
    $course_field['term'] = $db_info['columns']['value']['column'];
    $sql = sprintf("SELECT DISTINCT c.nid AS course_nid " .
        "FROM %s c JOIN %s t ON (c.nid = t.nid AND c.vid = t.vid) " .
        "JOIN node n ON (n.nid = t.nid AND n.vid = t.vid AND n.type = 'pc_course') " .
        "WHERE c.%s = '%s' AND  t.%s = '%s'",
        $course_table['course_id'],
        $course_table['term'],
        $course_field['course_id'],
        $course_id,
        $course_field['term'],
        $term);

    // echo 'course lookup sql: '.$sql.chr(10).chr(13);

    $result = db_query($sql);

    $course_nid = db_result($result);

    $course_node = node_load($course_nid);

    if ($course_nid) {
        return $course_node;
    } else {
        // echo 'Course not found: '.$course_id.' - '.$term.chr(10).chr(13);
        return null;
    }

} // function penncourse_load_course_node($subj_code)

function penncourse_log_exception(Exception $e, $message) {
    watchdog('penncourse cron', 'exception: ' . $e->getMessage() . ' - ' . $message, array(), WATCHDOG_NOTICE);
}

