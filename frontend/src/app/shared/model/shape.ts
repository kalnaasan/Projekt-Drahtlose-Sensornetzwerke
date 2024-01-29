import {Element} from "./element";

export interface Shape {
  id: string;
  height: number;
  width: number;
  elements: Element[];
  createdAt: Date;
  updatedAt: Date;
}
