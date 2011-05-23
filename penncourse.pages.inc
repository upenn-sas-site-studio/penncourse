<?php
// $Id: penncourse.pages.inc,v 1.1 2011/05/23 19:57:47 rickward Exp $

/**
 * @file
 * Page callbacks for the Curriculum module
 */

/*
 * PAGE CALLBACKS
 */

/**
 * function penncourse_course_list_term($term)
 * 
 * menu callback - display the list of courses for a given term
 * 
 * if $subj_code is given, include Subject Area description
 */
function penncourse_course_list_term($term,$subj_code=NULL) {
  drupal_add_css(drupal_get_path('module','penncourse').'/penncourse.css');
  $breadcrumb = array(l(t('Home'), NULL));
  if ($subj_code) {
    $breadcrumb[] = l(t('All courses for '.penncourse_translate_term($term)), 'pc/term/'.$term);
    drupal_set_title(penncourse_translate_subject($subj_code).' courses for '.penncourse_translate_term($term));
  } else {
    drupal_set_title('Courses for '.penncourse_translate_term($term));
  }
  drupal_set_breadcrumb($breadcrumb);

  $output = '';
  if ($subj_code) {
    $output .= views_embed_view('pc_subj_area_descr','default',$subj_code);
  }
  $output .= views_embed_view('pc_section_table','default',$term,$subj_code);
  
  return $output;
} // function penncourse_course_list_term()

/**
 * function penncourse_course_detail($term,$course_id)
 * 
 * menu callback - display the detailed course listing (with sections) for a given course/term
 */
function penncourse_course_detail($term,$course_id) {
  drupal_add_css(drupal_get_path('module','penncourse').'/penncourse.css');
  $node = penncourse_load_course_node($course_id,$term);
  
  drupal_set_title(check_plain($node->title));
  drupal_set_breadcrumb(
    array(
      l(t('Home'), NULL), 
      l(t('All courses for '.penncourse_translate_term($term)), 'pc/term/'.$term), 
      l(t(penncourse_translate_subject($node->field_pc_subj_area[0]['value']).' courses for '.penncourse_translate_term($term)), 'pc/term/'.$term.'/subject/'.$node->field_pc_subj_area[0]['value'])
    )
  );

  $output = '';
  $output .= views_embed_view('pc_course_detail','default',$term,$course_id);
  
  return $output;
} // function penncourse_course_detail()

/**
 * function penncourse_load_year($year)
 * 
 * load the course data for a given year
 */
function penncourse_load_year($year) {
  // check for valid year - no data loaded for years before 2008
  if (!preg_match('/^[0-9]{4}$/',$year) || (($year < 2008) || ($year > date('Y',time())))) {
    $output = 'Invalid year specified.';
    return $output;
    // drupal_not_found();
  } else {
    // get subject areas
    $subject_array = explode(' ',trim(variable_get('penncourse_subject_areas', '')));
    
    // process each subject area as set on the penncourse system settings form
    foreach ($subject_array as $subject) {
      penncourse_process_subj_area($subject,$year);
    }
    $output = 'Courses loaded for '.$year.'!';
    return $output;
  }
  
} // function penncourse_load_year($year)
