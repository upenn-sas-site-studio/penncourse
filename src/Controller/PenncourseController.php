<?php

namespace Drupal\penncourse\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Config\ConfigFactory;
use Drupal\penncourse\Service\PenncourseService;

/**
 * Class PenncourseController.
 */
class PenncourseController extends ControllerBase {

  /**
   * The Penncourse helper service.
   *
   * @var \Drupal\penncourse\Service\PenncourseService
   */
  protected $penncourse;

  /**
   * Drupal\Core\Config\ConfigFactory definition.
   *
   * @var \Drupal\Core\Config\ConfigFactory
   */
  protected $configFactory;

  /**
   * Constructs a new PenncourseController object.
   */
  // public function __construct(ConfigFactory $config_factory) {
  public function __construct(ConfigFactory $config_factory, PenncourseService $penncourse) {
    $this->configFactory = $config_factory;
    $this->penncourse = $penncourse;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    $config_factory = $container->get('config.factory');
    $penncourse = $container->get('penncourse.service');
    return new static(
      $config_factory,
      $penncourse
    );
  }

  /**
   * Test.
   *
   * @return string
   *   Return Hello string.
   */
  public function test() {
    $subj_areas = $this->penncourse->test();
    // $subj_areas = \Drupal::service('penncourse.service')->test();

    return [
      '#type' => 'markup',
      '#markup' => $subj_areas
    ];
  }

  /**
   * View courses
   *
   * @return array
   *   Return course listings.
   */
  public function viewCourseTable($term, $subj_code = 'all', $level = 'all') {
    // module_load_include('inc', 'penncourse', 'penncourse.include');
    // drupal_add_js(drupal_get_path('module', 'penncourse') . '/js/penncourse_course_table.js');
    // $breadcrumb = array(l(t('Home'), NULL));

    kint($this->penncourse->getSectionParams());

    // if term is null, set to current term
    if (!$term) {
        $term = $this->penncourse->getFinalTerm();
    }

    $section_data = $this->penncourse->getTermSections('2018A', 'ECON');
    kint($section_data);

    // build title string and extra breadcrumb, if needed
    /* $title = '';
    if ((!$level || ($level == 'all')) && (!$subj_code || ($subj_code == 'all'))) {
        $title = 'Courses for ';
    }
    else {
        $breadcrumb[] = l(t('All courses for ' . penncourse_translate_term($term)), 'pc/term/' . $term);
        if ($subj_code && ($subj_code != 'all')) {
            $title = penncourse_translate_subject($subj_code);
        }
        if (($level == 'graduate') || ($level == 'undergraduate')) {
            $title = ucfirst($level) . ' ' . $title;
        }
        $title = $title . ' courses for ';
    }
    $title = $title . penncourse_translate_term($term);
    drupal_set_title(t($title));
    drupal_set_breadcrumb($breadcrumb); */

    /* $output = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => 'mytable-order'),
    )); */

    $output = array();

    return $output;
  }
}
