import {AfterViewInit, Component, OnInit} from '@angular/core';
import {FormArray, FormBuilder, FormGroup, Validators} from "@angular/forms";
import {Shape} from "../../../shared/model/shape";
import * as d3 from 'd3';

@Component({
  selector: 'app-plan-dialog',
  templateUrl: './plan-dialog.component.html',
  styleUrls: ['./plan-dialog.component.scss']
})
export class PlanDialogComponent implements OnInit, AfterViewInit {

  public formGroup !: FormGroup;
  public shape!: Shape;
  protected svg: any;

  constructor(private formBuilder: FormBuilder) {
    this.formGroup = this.formBuilder.group({
      width: ['', Validators.required],
      height: ['', Validators.required],
      elements: this.formBuilder.array([]),
    });
    this.shape = {
      id: '',
      width: 300,
      height: 300,
      elements: [
        {
          id: '',
          type: 'Line',
          start: '10:010',
          end: '280:010',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '280:010',
          end: '280:110',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '280:130',
          end: '280:280',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '280:280',
          end: '10:280',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '10:280',
          end: '10:010',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '140:010',
          end: '140:140',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '280:090',
          end: '200:090',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '280:150',
          end: '220:150',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '70:280',
          end: '70:250',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '10:130',
          end: '50:130',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '70:070',
          end: '0:0',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '200:45',
          end: '0:0',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '220:240',
          end: '0:0',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '50:200',
          end: '0:0',
          shape: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
      ],
      createdAt: new Date(),
      updatedAt: new Date(),
    }
    this.addShapeToFromGroup();
  }

  ngOnInit(): void {
    // this.addElement('Line');
    this.createShape();
    console.log(this.svg);
    /*for (let i = 0; i < this.elementControls.length; i++) {
      this.addElementToSVG(i);
    }
    this.addShapeToSVG();*/
  }
  ngAfterViewInit(): void {
    this.createShape();
    console.log(this.svg);
    for (let i = 0; i < this.elementControls.length; i++) {
      this.addElementToSVG(i);
    }
  }

  createShape() {
    const width = this.formGroup.get('width')?.value;
    const height = this.formGroup.get('height')?.value;
    console.log(width);
    this.svg = d3.select('#svg-container-dialog').append('svg')
      .attr('width', width)
      .attr('height', height);
  }

  get elements() {
    return this.formGroup.controls["elements"] as FormArray;
  }

  get elementControls() {
    return (this.formGroup.controls["elements"] as FormArray).controls as FormGroup[];
  }

  addElement(type: string) {
    const elementFrom = this.formBuilder.group({
      type: [type, Validators.required],
      start: ['', Validators.required],
      end: ['', Validators.required]
    });
    this.elements.push(elementFrom);
  }

  addElementToSVG(index: number) {
    if (!this.svg) {
      this.createShape();
    }

    const element = this.formGroup.get('elements')?.value[index];
    console.log(element);
    if (element.type.toLowerCase() === 'line') {
      this.svg.append('line')
        .attr('id', 'line-' + index)
        .attr('x1', element.start.split(':')[0])
        .attr('y1', element.start.split(':')[1])
        .attr('x2', element.end.split(':')[0])
        .attr('y2', element.end.split(':')[1])
        .attr('stroke', 'gray')
        .attr('stroke-width', 3);
    } else if (element.type.toLowerCase() === 'sensor') {
      const circleRadius = 15;
      this.svg.append('circle')
        .attr('id', 'circle-' + index)
        .attr('class', 'cursor-pointer')
        .attr('cx', element.start.split(':')[0])
        .attr('cy', element.start.split(':')[1])
        .attr('r', circleRadius)
        .attr('fill', index % 2 === 0 ? 'blue' : 'red');
    }

  }

  addShapeToFromGroup(): void {
    this.formGroup.get('width')?.setValue(this.shape.width);
    this.formGroup.get('height')?.setValue(this.shape.height);
    for (const item of this.shape.elements) {
      const elementFrom = this.formBuilder.group({
        type: [item.type, Validators.required],
        start: [item.start, Validators.required],
        end: [item.end, Validators.required]
      });
      this.elements.push(elementFrom);
    }
  }
}
