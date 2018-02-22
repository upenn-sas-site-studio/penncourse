<?php

namespace Drupal\penncourse\Service;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Site\Settings;

/**
 * Class PenncourseService.
 */
// class PenncourseService implements PenncourseServiceInterface {
class PenncourseService {

  protected $config;
  protected $logger;

  /**
   * Constructs a new PenncourseService object.
   */
  public function __construct() {
    $this->config = \Drupal::service('config.factory')->getEditable('penncourse.penncourseconfig');
    $this->logger = \Drupal::service('logger.factory')->get('penncourse');
  }

  public function test() {
    kint($this->config->get('penncourse_subject_map'));
    kint($this->config->get('penncourse_available_terms'));
    return $this->config->get('penncourse_authorization_token');
  }

  public function getSectionParams() {
    $curl = curl_init();
    curl_setopt_array($curl, array(
        CURLOPT_HTTPHEADER => array(
            'Authorization-Bearer: ' . $this->config->get('penncourse_authorization_bearer'),
            'Authorization-Token: ' . $this->config->get('penncourse_authorization_token')
        ),
        CURLOPT_RETURNTRANSFER => 1,
        CURLOPT_URL => 'https://esb.isc-seo.upenn.edu/8091/open_data/course_section_search_parameters/'
    ));

    $curl_return = curl_exec($curl);
    $result = json_decode($curl_return);
    // watchdog('penncourse test', $curl_return, array(), WATCHDOG_NOTICE);

    $service_error = $result->service_meta->error_text;
    if (!$service_error && isset($result->result_data[0])) {
        return $result->result_data[0];
    }
    else {
        return (object) ['error_text' => $service_error];
    }
  }

  /**
   * function penncourse_update_section_params
   *
   * updates the variable penncourse_subject_map and penncourse_available_terms
   * with the latest parameters from the section search web service
   */
  public function updateSectionParams() {
    $result = $this->getSectionParams();
    $this->logger->notice('penncourse cron: updating section service parameters');

    if (!isset($result->error_text)) {
      $subject_array = (array) $result->departments_map;
      $terms_array = (array) $result->available_terms_map;
      $this->config->set('penncourse_subject_map', $subject_array);
      $this->config->set('penncourse_available_terms', $terms_array);
      $this->config->save();
    }
    else {
      $this->logger->error('penncourse cron: exception: ' . $result->error_text);
    }
  }

  /**
   * process the section entries for a single subject area
   * @param $subj_area
   * @param $year
   */
  public function processSubjectArea($subj_area) {
    $terms = $this->config->get('penncourse_available_terms');

    foreach ($terms as $key => $term) {
      loadSectionsBySubject($key, $subj_area);
    }
  }

  /**
   * call the web service to retrieve section information
   * @param $term
   * @param $subj_area
   * @param $page
   */
  function callSectionService($term, $subj_area, $page = 1) {
    if ($term && $subj_area) {
        $curl = curl_init();
        curl_setopt_array($curl, array(
            CURLOPT_HTTPHEADER => array(
              'Authorization-Bearer: ' . $this->config->get('penncourse_authorization_bearer'),
              'Authorization-Token: ' . $this->config->get('penncourse_authorization_token')
            ),
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_URL => 'https://esb.isc-seo.upenn.edu/8091/open_data/course_section_search?course_id=' . $subj_area . '&term=' . $term . '&page_number=' . $page . '&number_of_results_per_page=100'
        ));

        $curl_return = curl_exec($curl);

        $result = json_decode($curl_return);

        $service_error = $result->service_meta->error_text;
        if (!$service_error && isset($result->result_data)) {
            return $result;
            // return $result->result_data;
        }
        else {
            return (object) ['error_text' => $service_error];
        }
    }
    else {
        // term and/or subject area not supplied
        return (object) ['error_text' => 'A valid term code or subject area must be supplied'];
    }
  }

  /**
   * retrieve all section data for a given term and subject area
   * @param $term
   * @param $subj_area
   */
  public function getTermSections($term, $subj_area) {
    $course_data = array();

    // get first data set
    try {
        $result = $this->callSectionService($term, $subj_area, 1);
        if (isset($result->error_text)) {
            throw new Exception("Course section web service error: " . $result->error_text, 1);
        }
        else {
            // add course data to the array to be returned
            $course_data = array_merge($course_data, $result->result_data);

            if ($result->service_meta->number_of_pages > 1) {
                // there is more than 1 page of data, we need to get the rest
                $current_page = 1;
                $next_page = 2;

                while ($next_page > $current_page) {
                    $result = $this->callSectionService($term, $subj_area, $next_page);
                    if (isset($result->error_text)) {
                        $next_page = $current_page; // we don't want to continue looping on an error
                        throw new Exception("Course section web service error: " . $result->error_text, 1);
                    }
                    else {
                        // add course data to the array to be returned
                        $course_data = array_merge($course_data, $result->result_data);
                        $next_page = $result->service_meta->next_page_number;
                        $current_page = $result->service_meta->current_page_number;
                    }
                }
            }
        }
    }
    catch (Exception $e) {
      $this->logger->error('penncourse exception: ' . $e, 'term: ' . $term . '; subject area: ' . $subj_area);
    }
    return $course_data;
  } // function penncourse_retrieve_term_sections

  /**
   * return the term code for the most currently published course roster
   *
   * course rosters for the summer and fall are finalized on 2/19
   * rosters for the spring are finalized on 10/1
   * @return string
   *   Return current term code.
   *
   */
  public function getFinalTerm() {
    if ((date('n', REQUEST_TIME) >= 10) && (date('j', REQUEST_TIME) >= 1)) {
      // date is after 10/1 - final term is next spring term
      $term = date('Y', REQUEST_TIME) + 1;
      $term .= 'A';
    }
    elseif ((date('n', REQUEST_TIME) >= 2) && ((date('j', REQUEST_TIME) >= 19) || (date('n', REQUEST_TIME) >= 3))) {
      // date is after 2/19 - final term is next (or current) fall term
      $term = date('Y', REQUEST_TIME);
      $term .= 'C';
    }
    else {
      // date is before 2/19 - final term is current spring term
      $term = date('Y', REQUEST_TIME);
      $term .= 'A';
    }
    return $term;
  }
}
